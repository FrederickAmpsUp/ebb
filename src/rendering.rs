use wgpu;
use crate::Instance;
use crate::ecs;

pub struct RenderContext {
    encoder: wgpu::CommandEncoder
}

pub struct RenderMesh {
    // TODO: stuff
}
impl ecs::Component for RenderMesh {}

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

        render_ctx.clear(&view, self.clear_color);

        for entity in world {
            if entity.has_component::<RenderMesh>() {
                // TODO: render mesh
            }
        }

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

        // TODO: higher level of abstraction
    pub fn create_render_pass_raw(&mut self, desc: &wgpu::RenderPassDescriptor) -> wgpu::RenderPass {
        self.encoder.begin_render_pass(desc)
    }

    pub fn clear(&mut self, surf_view: &wgpu::TextureView, color: wgpu::Color) {
        let _rp = self.create_render_pass_raw(&wgpu::RenderPassDescriptor {
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
        });
    }

    pub fn to_command_buffer(self) -> wgpu::CommandBuffer {
        self.encoder.finish()
    }

    pub fn submit<'a>(self, instance: &Instance<'a>) {
        instance.raw_queue().submit(std::iter::once(self.to_command_buffer()));
    }
}