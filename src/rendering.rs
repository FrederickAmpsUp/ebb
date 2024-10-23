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

pub struct RenderContext {
    encoder: wgpu::CommandEncoder
}

pub struct RenderPipeline {
    pipeline: wgpu::RenderPipeline
}
impl RenderPipeline {
    pub fn from_raw(instance: &Instance, desc: &wgpu::RenderPipelineDescriptor) -> Self {
        Self {
            pipeline: instance.raw_device().create_render_pipeline(desc)
        }
    }

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

    pub fn for_mesh<V: mesh::Vertex>(instance: &Instance, shader: wgpu::ShaderModuleDescriptor) -> Self {
        Self::new(instance, &[V::LAYOUT], shader)
    }

    pub fn raw_pipeline(&self) -> &wgpu::RenderPipeline {
        &self.pipeline
    }
}

pub struct BasicRenderSystem {
    instance: Instance<'static>,
    clear_color: wgpu::Color
}
impl ecs::System for BasicRenderSystem {
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
    pub fn new(instance: Instance<'static>, clear_color: wgpu::Color) -> Self {
        Self {
            instance, clear_color
        }
    }
}

impl RenderContext {
    pub fn new<'a>(instance: &Instance<'a>) -> Self {
        Self {
            encoder: instance.raw_device().create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("Ebb Builtin RenderContext - Encoder")
            })
        }
    }

    pub fn create_render_pass_raw(&mut self, desc: &wgpu::RenderPassDescriptor) -> wgpu::RenderPass {
        self.encoder.begin_render_pass(desc)
    }

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

    pub fn to_command_buffer(self) -> wgpu::CommandBuffer {
        self.encoder.finish()
    }

    pub fn submit<'a>(self, instance: &Instance<'a>) {
        instance.raw_queue().submit(std::iter::once(self.to_command_buffer()));
    }
}