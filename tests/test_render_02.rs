use log::*;
use wasm_bindgen::prelude::*;
use std::rc::Rc;
use ebb;

#[async_std::main]
#[wasm_bindgen(start)]
async fn main() {
    ebb::setup();

    let winit_window;
    {
        let name = String::from("Ebb Test Window");
        let mut window = ebb::window::new((1920, 1080), &name, false);

        winit_window = window.raw_window().clone();

        #[allow(unused_assignments)]
        let mut size = winit_window.inner_size();

        let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
            backends: wgpu::Backends::all(),
            ..Default::default()
        });

        {
            let surface = Rc::new(instance.create_surface(winit_window.as_ref()).unwrap());

            let adapter = instance.request_adapter(
                &wgpu::RequestAdapterOptions {
                    power_preference: wgpu::PowerPreference::default(),
                    compatible_surface: Some(&surface),
                    force_fallback_adapter: false,
                    ..Default::default()
                }
            ).await.expect("Ebb: Failed to find a suitable adapter!");

            info!("Ebb: Found device: {}", adapter.get_info().name);
            info!("Ebb: \tType:    {:?}", adapter.get_info().device_type);
            info!("Ebb: \tBackend: {}", adapter.get_info().backend);

            let (_dev, queue) = adapter.request_device(
                &wgpu::DeviceDescriptor {
                    required_features: wgpu::Features::empty(),
                    required_limits: wgpu::Limits::downlevel_webgl2_defaults(),
                    label: None,
                    memory_hints: Default::default()
                }, None
            ).await.expect("Ebb: Failed to create device!");

            let device = Rc::new(_dev);

            let surface_caps = surface.get_capabilities(&adapter);
            
            let surface_format = surface_caps.formats.iter()
                .find(|f| f.is_srgb())
                .copied()
                .unwrap_or(surface_caps.formats[0]);

            let mut config = wgpu::SurfaceConfiguration {
                usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
                format: surface_format,
                width: size.width,
                height: size.height,
                present_mode: surface_caps.present_modes[0],
                alpha_mode: surface_caps.alpha_modes[0],
                view_formats: vec![],
                desired_maximum_frame_latency: 2,
            };

            surface.configure(&device, &config);

            window.on_render({
                let surface = Rc::clone(&surface);
                let device = Rc::clone(&device);
                
                move || {
                    let output = surface.get_current_texture().unwrap();

                    let view = output.texture.create_view(&wgpu::TextureViewDescriptor::default());

                    let mut encoder = device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
                        label: Some("Render Encoder"),
                    });

                    {
                        let _render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                            label: Some("Render Pass"),
                            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                                view: &view,
                                resolve_target: None,
                                ops: wgpu::Operations {
                                    load: wgpu::LoadOp::Clear(wgpu::Color {
                                        r: 0.3,
                                        g: 0.2,
                                        b: 0.1,
                                        a: 1.0,
                                    }),
                                    store: wgpu::StoreOp::Store,
                                },
                            })],
                            depth_stencil_attachment: None,
                            occlusion_query_set: None,
                            timestamp_writes: None,
                        });
                    }
                
                    // submit will accept anything that implements IntoIter
                    queue.submit(std::iter::once(encoder.finish()));
                    output.present();
            }});

            window.on_resize({
                let surface = Rc::clone(&surface);
                let device = Rc::clone(&device);

                move |new_size| {
                    info!("resized {:?}", new_size);
                    if new_size.width > 0 && new_size.height > 0 {
                        size = *new_size;
                        config.width = size.width;
                        config.height = size.height;
                        surface.configure(&device, &config);
                    }
            }});
    
        }

        window.run();
    }
}
