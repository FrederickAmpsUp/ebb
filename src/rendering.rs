use wgpu;
use crate::Instance;

pub struct RenderContext {
    encoder: wgpu::CommandEncoder
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