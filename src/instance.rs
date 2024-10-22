use wgpu;
use winit::dpi::PhysicalSize;
use std::sync::Arc;
use crate::surface::Surface;

#[allow(dead_code, reason = "annoying warnings")]
pub struct Instance<'a> {
    instance: wgpu::Instance,
    surface: wgpu::Surface<'a>,
    device: wgpu::Device,
    queue: wgpu::Queue,

    size: winit::dpi::PhysicalSize<u32>,

    config: wgpu::SurfaceConfiguration,

    window: Arc<winit::window::Window>,
}

impl<'a> Instance<'a> {
    pub fn new(window: Arc<winit::window::Window>) -> Self {
        let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
            backends: wgpu::Backends::all(),
            ..Default::default()
        });

        // TODO: proper error handling

        let surface: wgpu::Surface<'a> = instance.create_surface(window.clone()).expect("Ebb: Failed to create window surface!");
        let adapter = futures::executor::block_on(instance.request_adapter(
            &wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                compatible_surface: Some(&surface),
                force_fallback_adapter: false,
               ..Default::default()
            }
        )).expect("Ebb: failed to request adapter!");

        let (device, queue) = futures::executor::block_on(adapter.request_device(
            &wgpu::DeviceDescriptor {
                required_features: wgpu::Features::empty(),
                required_limits: wgpu::Limits::downlevel_webgl2_defaults(),
                label: None,
                memory_hints: Default::default()
            }, None
        )).expect("Ebb: Failed to get device handle!");

        let surface_caps = surface.get_capabilities(&adapter);

        let surface_format = surface_caps.formats.iter().find(|f| !f.is_srgb()).copied().unwrap_or(surface_caps.formats[0]); // TODO: better surface format selection with a score function

        let size = window.inner_size();

        let config = wgpu::SurfaceConfiguration {
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
            format: surface_format,
            width: size.width,
            height: size.height,
            // TODO: present mode selection
            present_mode: surface_caps.present_modes[0],
            alpha_mode: surface_caps.alpha_modes[0],
            view_formats: vec![],
            desired_maximum_frame_latency: 2,
        };

        surface.configure(&device, &config);

        Self {
            instance, surface, device, queue, size, config, window
        }
    }

    pub fn resize(&mut self, new_size: &PhysicalSize<u32>) {
        let mut new_size = *new_size;

        let max_extent = self.device.limits().max_texture_dimension_2d;

        if new_size.width > max_extent {
            new_size.width = max_extent;
            let _ = self.window.request_inner_size(new_size);
            return;
        }
        if new_size.height > max_extent {
            new_size.height = max_extent;
            let _ = self.window.request_inner_size(new_size);
            return;
        }

        if new_size.width > 0 && new_size.height > 0 && new_size.width < max_extent && new_size.height < max_extent {
            self.config.width = new_size.width;
            self.config.height = new_size.height;
            self.surface.configure(&self.device, &self.config);
        }
    }

    pub fn raw_device(&self) -> &wgpu::Device {
        &self.device
    }

    pub fn raw_queue(&self) -> &wgpu::Queue {
        &self.queue
    }

    pub fn raw_config(&self) -> &wgpu::SurfaceConfiguration {
        &self.config
    }

    pub fn window_surface(&'a self) -> Surface<'a> {
        Surface::new(&self.surface)
    }
}