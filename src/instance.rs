use super::window;
use wgpu;
use std::rc::Rc;

#[allow(dead_code, reason = "annoying warnings")]
pub struct Instance<'a> {
    window: &'a mut window::Window<'a>,
    instance: wgpu::Instance,
    surface: wgpu::Surface<'a>,
    device: wgpu::Device,
    queue: wgpu::Queue,
}

impl<'a> Instance<'a> {
    pub async fn new(window: &'a mut window::Window<'a>) -> Self {
        let instance = wgpu::Instance::new(wgpu::InstanceDescriptor {
            backends: wgpu::Backends::all(),
            ..Default::default()
        });

        // TODO: proper error handling

        let wnt_window = window.raw_window();
        let surface: wgpu::Surface<'a> = instance.create_surface(wnt_window.as_ref()).expect("Ebb: failed to create surface!");

        let adapter = instance.request_adapter(
            &wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                compatible_surface: Some(&surface),
                force_fallback_adapter: false,
               ..Default::default()
            }
        ).await.expect("Ebb: failed to request adapter!");

        let (device, queue) = adapter.request_device(
            &wgpu::DeviceDescriptor {
                required_features: wgpu::Features::empty(),
                required_limits: wgpu::Limits::downlevel_webgl2_defaults(),
                label: None,
                memory_hints: Default::default()
            }, None
        ).await.expect("Ebb: Failed to get device handle!");

        Self {
            window,
            instance,
            surface,
            device,
            queue
        }
    }

    pub fn raw_queue(&self) -> &wgpu::Queue {
        &self.queue
    }

    pub fn window(&self) -> &window::Window<'a> {
        &self.window
    }

    pub fn window_mut(&mut self) -> &mut window::Window<'a> {
        &mut self.window
    }
}