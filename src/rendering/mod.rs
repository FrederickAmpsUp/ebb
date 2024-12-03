use wgpu;
use bytemuck;
use crate::camera;
use crate::transform;
use crate::Instance;
use crate::ecs;
use crate::mesh;
use wgpu::util::DeviceExt;
use glam;
use std::sync::Arc;

pub mod shader;

/*
 * TODO:
 * - Shader abstraction and preprocesser (#include directives and prelude)
 * - Better uniform handling system
 *      uniforms will be parsed from shader code and may be accessed by name
 * - More configurability for render pipelines
 * - Better abstraction for RenderContext
*/

pub trait ShaderUniform: Copy + Clone + bytemuck::Pod + bytemuck::Zeroable {}

#[repr(C)]
#[derive(Copy, Clone)]
pub struct BuiltinUniforms {
    projection_mtx: glam::Mat4,
    view_mtx: glam::Mat4
}
unsafe impl bytemuck::Pod for BuiltinUniforms {}
unsafe impl bytemuck::Zeroable for BuiltinUniforms {}

/// An interface used for creating command buffers for rendering.
pub struct RenderContext {
    encoder: wgpu::CommandEncoder
}

/// A collection of shader inputs (vertices) and color attachments (framebuffers)
pub struct RenderPipeline {
    pipeline: wgpu::RenderPipeline,
    builtin_uniforms: wgpu::Buffer,
    builtin_uniform_bind_group: wgpu::BindGroup,
    texture: Arc<dyn crate::texture::Texture> 
}
impl RenderPipeline {
    /// Creates a render pipeline from the raw WGPU descriptor.
    /// # Arguments
    /// 
    /// * `instance` - The instance to use when creating the pipeline.
    /// * `desc` - The WGPU descriptor to use when creating the pipeline.
    /// 
    /// # Returns
    /// 
    /// The created render pipeline.
    /// 
    /// # Examples
    /// 
    /// ```ignore
    /// let pipeline = ebb::rendering::RenderPipeline::from_raw(&g_instance, &descriptor);
    /// ```
    pub fn from_raw(instance: &Instance, desc: &wgpu::RenderPipelineDescriptor, uubg: wgpu::BindGroup, uu: wgpu::Buffer, texture: Arc<dyn crate::texture::Texture>) -> Self {
        Self {
            pipeline: instance.raw_device().create_render_pipeline(desc),
            builtin_uniform_bind_group: uubg,
            builtin_uniforms: uu,
            texture
        }
    }

    /// Creates a render pipeline from the vertex configuration and shader.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - The instance to use when creating the pipeline.
    /// * `buffers` - The set of vertex buffer layouts to use.
    /// * `shader` - The shader module (`vs_main` `fs_main` required) to use for rendering.
    /// 
    /// # Returns
    /// 
    /// The created render pipeline.
    /// 
    /// # Examples
    /// 
    /// ```ignore
    /// let pipeline = ebb::rendering::RenderPipeline::new(&g_instance, &[MeshVertex::LAYOUT], wgpu::include_wgsl!("shaders/example.wgsl"));
    /// ```
    pub fn new(instance: &Instance, buffers: &[wgpu::VertexBufferLayout], shader: wgpu::ShaderModuleDescriptor, texture: Arc<dyn crate::texture::Texture>) -> Self {
        let shader = instance.raw_device().create_shader_module(shader);

        let b_uniform_buffer = instance.raw_device().create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Ebb user uniform buffer"),
                contents: &[0 as u8; size_of::<BuiltinUniforms>()],
                usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST
            }
        );

        let b_uniform_bind_group_layout = instance.raw_device().create_bind_group_layout(
            &wgpu::BindGroupLayoutDescriptor {
                entries: &[
                    wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::VERTEX_FRAGMENT,
                        ty: wgpu::BindingType::Buffer {
                            ty: wgpu::BufferBindingType::Uniform,
                            has_dynamic_offset: false,
                            min_binding_size: None
                        },
                        count: None,
                    }
                ],
                label: Some("Ebb user uniform bindgroup")
            }
        );

        let b_uniform_bind_group = instance.raw_device().create_bind_group(
            &wgpu::BindGroupDescriptor {
                layout: &b_uniform_bind_group_layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: b_uniform_buffer.as_entire_binding()
                    }
                ],
                label: Some("Ebb user uniform bindgroup")
            }
        );

        let layout = instance.raw_device().create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("Ebb Builtin RenderPipeline - PipelineLayout"),
            bind_group_layouts: &[
                &b_uniform_bind_group_layout,
                &texture.raw_bind_group_layout()
            ],
            push_constant_ranges: &[],
        });

        let render_pipeline = instance.raw_device().create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: Some("Ebb Builtin RenderPipeline - RenderPipeline"),
            layout: Some(&layout),
            vertex: wgpu::VertexState {
                module: &shader,
                entry_point: "vs_main",
                buffers,
                compilation_options: wgpu::PipelineCompilationOptions::default(),
            },
            fragment: Some(wgpu::FragmentState {
                module: &shader,
                entry_point: "fs_main",
                targets: &[Some(wgpu::ColorTargetState {
                    format: instance.raw_config().format,
                    blend: Some(wgpu::BlendState::REPLACE),
                    write_mask: wgpu::ColorWrites::ALL,
                })],
                compilation_options: wgpu::PipelineCompilationOptions::default(),
            }),
            primitive: wgpu::PrimitiveState {
                topology: wgpu::PrimitiveTopology::TriangleList,
                strip_index_format: None,
                front_face: wgpu::FrontFace::Ccw,
                cull_mode: Some(wgpu::Face::Front),
                // Setting this to anything other than Fill requires Features::NON_FILL_POLYGON_MODE
                polygon_mode: wgpu::PolygonMode::Fill,
                // Requires Features::DEPTH_CLIP_CONTROL
                unclipped_depth: false,
                // Requires Features::CONSERVATIVE_RASTERIZATION
                conservative: false,
            },
            depth_stencil: Some(wgpu::DepthStencilState {
                format: wgpu::TextureFormat::Depth32Float,
                depth_write_enabled: true,
                depth_compare: wgpu::CompareFunction::Less,
                stencil: wgpu::StencilState::default(),
                bias: wgpu::DepthBiasState::default(),
            }),
            multisample: wgpu::MultisampleState {
                count: 1,
                mask: !0,
                alpha_to_coverage_enabled: false,
            },
            multiview: None,
            cache: None,
        });
        
        Self {
            pipeline: render_pipeline,
            builtin_uniforms: b_uniform_buffer,
            builtin_uniform_bind_group: b_uniform_bind_group,
            texture
        }
    }

    /// Creates a render pipeline for a mesh with the specified vertex type.
    /// 
    /// # Type Parameters
    /// 
    /// * `V` - a [mesh::Vertex], the layout of which is used as the vertex buffer in slot 0.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - The instance to use when creating the pipeline.
    /// * `shader` - The shader module (`vs_main` `fs_main` required) to use for rendering.
    /// 
    /// # Returns
    /// 
    /// The created render pipeline.
    /// 
    /// # Examples
    /// 
    /// ```ignore
    /// let pipeline = ebb::rendering::RenderPipeline::for_mesh::<MeshVertex>(&g_instance, wgpu::include_wgsl("shaders/example.wgsl"));
    /// ```
    pub fn for_mesh<V: mesh::Vertex>(instance: &Instance, shader: wgpu::ShaderModuleDescriptor, texture: Arc<dyn crate::texture::Texture>) -> Self {
        Self::new(instance, &[V::LAYOUT], shader, texture)
    }

    pub fn set_builtin_uniforms(&self, inst: &Instance, val: &BuiltinUniforms) {
        inst.raw_queue().write_buffer(&self.builtin_uniforms, 0, bytemuck::cast_slice(&[*val]));
        inst.raw_queue().submit([]);
    }

    pub fn get_uniforms_bind_group(&self) -> &wgpu::BindGroup {
        &self.builtin_uniform_bind_group
    }

    /// Get the raw WGPU pipeline object.
    /// 
    /// # Returns
    /// 
    /// A reference to the internal [wgpu::RenderPipeline].
    pub fn raw_pipeline(&self) -> &wgpu::RenderPipeline {
        &self.pipeline
    }

    pub fn get_texture(&self) -> &Arc<dyn crate::texture::Texture> {
        &self.texture
    }
}

/// An [ecs::System] for basic rendering tasks.
/// It clears the screen, then renders every entity with a RenderMesh component.
/// 
/// This struct takes ownership of an [Instance] which is used for rendering.
#[allow(dead_code)]
pub struct BasicRenderSystem {
    instance: Instance<'static>,
    depth_texture: wgpu::Texture,
    depth_texture_view: wgpu::TextureView,
    clear_color: wgpu::Color
}
impl ecs::System for BasicRenderSystem {

    /// Clears the screen and renders all entities with a RenderMesh component.
    /// 
    /// # Arguments
    /// 
    /// * `world` - The collection of entities to operate on.
    fn update(&self, world: &mut ecs::World) {
        let mut render_ctx = RenderContext::new(&self.instance);

        let renderables = &world.get_entities_with_all::<(mesh::RenderMesh,)>();

        for camera in world.get_entities_with_all::<(transform::Transform3D, camera::Camera)>() {
            let cam = camera.get_component::<camera::Camera>().unwrap();
            let trn = camera.get_component::<transform::Transform3D>().unwrap();

            let surf = cam.get_render_target();

            let mut render_pass = render_ctx.clear(&surf.borrow_mut().get_view(), Some(&self.depth_texture_view), self.clear_color);

            let pm = *cam.get_projection_matrix();
            let mm = trn.model_matrix;

            for entity in renderables {
                if let Some(component) = entity.get_component::<mesh::RenderMesh>() {
                    component.get_renderer().set_builtin_uniforms(&self.instance, &BuiltinUniforms{
                        projection_mtx: pm,
                        view_mtx: mm
                    });

                    render_pass.set_pipeline(&component.get_renderer().pipeline);
                    render_pass.set_bind_group(0, &component.get_renderer().builtin_uniform_bind_group, &[]);
                    
                    render_pass.set_bind_group(1, component.get_renderer().get_texture().raw_bind_group(), &[]);

                    let vb = component.get_vertex_buffer();
                    let ib = component.get_index_buffer();

                    render_pass.set_vertex_buffer(0, vb.slice(..));
                    render_pass.set_index_buffer(ib.slice(..), wgpu::IndexFormat::Uint32);
                    render_pass.draw_indexed(0..(component.get_num_indices() as u32), 0, 0..1);
                }
            }
            drop(render_pass);
        }

        render_ctx.submit(&self.instance);

        self.instance.window_surface().borrow_mut().present();
    }
}

impl BasicRenderSystem {
    /// Creates a new [BasicRenderSystem]
    /// 
    /// # Arguments
    /// 
    /// * `instance` - The [Instance] to use for rendering. Ownership is passed to the [BasicRenderSystem].
    /// * `clear_color` - The color to clear the screen to before rendering.
    /// 
    /// # Returns
    /// 
    /// The [BasicRenderSystem].
    pub fn new(instance: Instance<'static>, clear_color: wgpu::Color) -> Self {
        let size = wgpu::Extent3d {
            width: instance.raw_config().width.max(1),
            height: instance.raw_config().height.max(1),
            depth_or_array_layers: 1
        };

        let desc = wgpu::TextureDescriptor {
            label: Some("Ebb basic render system depth texture"),
            size,
            mip_level_count: 1,
            sample_count: 1,
            dimension: wgpu::TextureDimension::D2,
            format: wgpu::TextureFormat::Depth32Float,
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
            view_formats: &[]
        };
        let depth_texture = instance.raw_device().create_texture(&desc);
        let depth_texture_view = depth_texture.create_view(&Default::default());

        Self {
            instance, depth_texture, depth_texture_view, clear_color
        }
    }
}

impl RenderContext {

    /// Creates a new [RenderContext].
    /// 
    /// # Arguments
    /// 
    /// * `instance` - the [Instance] to use for rendering. Borrowed for the duration of the function execution.
    /// 
    /// # Returns
    /// 
    /// The created [RenderContext].
    pub fn new<'a>(instance: &Instance<'a>) -> Self {
        Self {
            encoder: instance.raw_device().create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("Ebb Builtin RenderContext - Encoder")
            })
        }
    }

    /// Creates a [wgpu::RenderPass] from a raw [wgpu::RenderPassDescriptor].
    /// Should generally only be used internally.
    /// 
    /// # Arguments
    /// 
    /// * `desc` - the [wgpu::RenderPassDescriptor] to use when creating the [wgpu::RenderPass].
    /// 
    /// # Returns
    /// 
    /// The [wgpu::RenderPass] matching the argument.
    pub fn create_render_pass_raw(&mut self, desc: &wgpu::RenderPassDescriptor) -> wgpu::RenderPass {
        self.encoder.begin_render_pass(desc)
    }

    /// Creates a [wgpu::RenderPass] which starts by clearing the input texture to a color.
    /// Should generally only be used internally.
    /// 
    /// # Arguments
    /// 
    /// * `surf_view` - the [wgpu::TextureView] to clear.
    /// * `color` - the [wgpu::Color] to clear the texture to.
    /// 
    /// # Returns
    /// 
    /// The [wgpu::RenderPass], ready for rendering, starting with a clear op.
    pub fn clear(&mut self, surf_view: &wgpu::TextureView, depth_stencil_view: Option<&wgpu::TextureView>, color: wgpu::Color) -> wgpu::RenderPass {
        let ds = if let Some(v) = depth_stencil_view {
            Some(wgpu::RenderPassDepthStencilAttachment {
                view: v,
                depth_ops: Some(wgpu::Operations {
                    load: wgpu::LoadOp::Clear(1.0),
                    store: wgpu::StoreOp::Store
                }),
                stencil_ops: None
            })
        } else {
            None
        };
        
        self.create_render_pass_raw(&wgpu::RenderPassDescriptor {
            label: Some("Ebb Builtin RenderContext - Clear"),
            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                view: surf_view,
                resolve_target: None,
                ops: wgpu::Operations {
                    load: wgpu::LoadOp::Clear(color),
                    store: wgpu::StoreOp::Store,
                },
            })],
            depth_stencil_attachment: ds,
            occlusion_query_set: None,
            timestamp_writes: None,
        })
    }

    /// Finishes rendering and creates a command buffer.
    /// This method consumes `self`.
    /// This should generally only be used internally.
    /// 
    /// # Returns
    /// 
    /// The [wgpu::CommandBuffer] which may be submit to the GPU for rendering.
    pub fn to_command_buffer(self) -> wgpu::CommandBuffer {
        self.encoder.finish()
    }

    /// Submits previously issued draw commands to the GPU for rendering.
    /// 
    /// # Arguments
    /// 
    /// * `instance` - the [Instance] to use for rendering. Borrowed for the duration of the function execution.
    pub fn submit<'a>(self, instance: &Instance<'a>) {
        instance.raw_queue().submit(std::iter::once(self.to_command_buffer()));
    }
}
