use wgpu;
use crate::Instance;
use crate::ecs;
use crate::mesh;

/*
 * TODO:
 * - Shader abstraction and preprocesser (#include directives and prelude)
 * - More configurability for render pipelines
 * - Better abstraction for RenderContext
*/

/// An interface used for creating command buffers for rendering.
pub struct RenderContext {
    encoder: wgpu::CommandEncoder
}

/// A collection of shader inputs (vertices) and color attachments (framebuffers)
pub struct RenderPipeline {
    pipeline: wgpu::RenderPipeline
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
    pub fn from_raw(instance: &Instance, desc: &wgpu::RenderPipelineDescriptor) -> Self {
        Self {
            pipeline: instance.raw_device().create_render_pipeline(desc)
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
    pub fn new(instance: &Instance, buffers: &[wgpu::VertexBufferLayout], shader: wgpu::ShaderModuleDescriptor) -> Self {
        let shader = instance.raw_device().create_shader_module(shader);
        let layout = instance.raw_device().create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("Ebb Builtin RenderPipeline - PipelineLayout"),
            bind_group_layouts: &[],
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
                cull_mode: Some(wgpu::Face::Back),
                // Setting this to anything other than Fill requires Features::NON_FILL_POLYGON_MODE
                polygon_mode: wgpu::PolygonMode::Fill,
                // Requires Features::DEPTH_CLIP_CONTROL
                unclipped_depth: false,
                // Requires Features::CONSERVATIVE_RASTERIZATION
                conservative: false,
            },
            depth_stencil: None,
            multisample: wgpu::MultisampleState {
                count: 1,
                mask: !0,
                alpha_to_coverage_enabled: false,
            },
            multiview: None,
            cache: None,
        });
        
        Self {
            pipeline: render_pipeline
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
    pub fn for_mesh<V: mesh::Vertex>(instance: &Instance, shader: wgpu::ShaderModuleDescriptor) -> Self {
        Self::new(instance, &[V::LAYOUT], shader)
    }

    /// Get the raw WGPU pipeline objects
    /// 
    /// # Returns
    /// 
    /// A reference to the internal [wgpu::RenderPipeline].
    pub fn raw_pipeline(&self) -> &wgpu::RenderPipeline {
        &self.pipeline
    }
}

/// An [ecs::System] for basic rendering tasks.
/// It clears the screen, then renders every entity with a RenderMesh component.
/// 
/// This struct takes ownership of an [Instance] which is used for rendering.
pub struct BasicRenderSystem {
    instance: Instance<'static>,
    clear_color: wgpu::Color
}
impl ecs::System for BasicRenderSystem {

    /// Clears the screen and renders all entities with a RenderMesh component.
    /// 
    /// # Arguments
    /// 
    /// * `world` - The collection of entities to operate on.
    fn update(&self, world: &mut Vec<ecs::Entity>) {
        let mut render_ctx = RenderContext::new(&self.instance);
        let output = self.instance
            .window_surface()
            .wgpu_surface()
            .get_current_texture()
            .expect("Ebb - Failed to acquire window surface texture");

        let view = output.texture.create_view(&wgpu::TextureViewDescriptor::default());

        let mut render_pass = render_ctx.clear(&view, self.clear_color);

        for entity in world {
            if let Some(component) = entity.get_component::<mesh::RenderMesh>() {
                render_pass.set_pipeline(&component.get_renderer().pipeline);
                let vb = component.get_vertex_buffer();
                let ib = component.get_index_buffer();

                render_pass.set_vertex_buffer(0, vb.slice(..));
                render_pass.set_index_buffer(ib.slice(..), wgpu::IndexFormat::Uint32);
                render_pass.draw_indexed(0..(component.get_num_indices() as u32), 0, 0..1);
            }
        }

        drop(render_pass);

        render_ctx.submit(&self.instance);
        output.present();
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
        Self {
            instance, clear_color
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
    pub fn clear(&mut self, surf_view: &wgpu::TextureView, color: wgpu::Color) -> wgpu::RenderPass {
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
            depth_stencil_attachment: None,
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