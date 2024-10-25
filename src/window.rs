use std::sync::Arc;

use winit::{
    event_loop::EventLoop,
    window::WindowBuilder,
    window::Window as WinitWindow,
    dpi::PhysicalSize,
    event::*,
    keyboard::*
};

/*
 * TODO:
 * --------------
 * Fullscreen windows
 * Input management
*/

use log::*;

/// A viewport for your application.
/// This could be a window on desktop platforms, a canvas on web platforms, or the screen on mobile/console.
/// 
/// Uses Winit internally.
pub struct Window<'a> {
    win: Arc<WinitWindow>,
    evt_loop: EventLoop<()>,

    render_fn: Box<dyn FnMut() + 'a>,
    resize_fn: Box<dyn FnMut(&PhysicalSize<u32>) + 'a>
}

impl<'a> Window<'a> {

    /// Creates a window from the raw Winit objects.
    /// 
    /// # Arguments
    /// 
    /// * `win` - the [winit::window::Window] to use.
    /// * `evt_loop` - the [winit::event_loop::EventLoop] to use.
    /// 
    /// # Returns
    /// 
    /// The [Window] wrapper over the inputs.
    pub fn from_raw(win: WinitWindow, evt_loop: EventLoop<()>) -> Self {
        Self {
            win: Arc::new(win),
            evt_loop,
            render_fn: Box::new(|| {}),
            resize_fn: Box::new(|_| {}),
        }
    }

    /// Creates a new window.
    /// This will use the `ebb-canvas` HTML div element on web platforms.
    /// 
    /// # Arguments
    /// 
    /// * `size` - The size of the window, in pixels.
    /// * `title` - The title of the window.
    /// * `resizable` - Wether or not resizing the window is allowed.
    /// 
    /// # Returns
    /// 
    /// The [Window] object.
    pub fn new(size: (u32, u32), title: String, resizable: bool) -> Window<'a> {
        debug!("Ebb: Creating window");
    
        let event_loop = EventLoop::new().unwrap();
        let window = WindowBuilder::new().build(&event_loop).unwrap();

        window.set_resizable(resizable);
        let _ = window.request_inner_size(PhysicalSize::new(size.0, size.1));
        window.set_title(&title);
    
        #[cfg(target_arch = "wasm32")]
        {
            debug!("Ebb: Using web canvas for window");


            let canvas = web_sys::Element::from(window.canvas().unwrap());
            canvas.set_attribute("width", &size.0.to_string()).unwrap();
            canvas.set_attribute("height", &size.1.to_string()).unwrap();
    
            use winit::platform::web::WindowExtWebSys;
            
            web_sys::window()
                .and_then(|win| win.document())
                .and_then(|doc| {
                    let dst = doc.get_element_by_id("ebb-canvas").expect("Cannot find Ebb canvas element");
    
                    let canvas = web_sys::Element::from(window.canvas()?);
                    dst.append_child(&canvas).ok()?;
                    Some(())
                })
                .expect("Couldn't append canvas to document body.");
            }
        #[cfg(not(target_arch = "wasm32"))]
        {
            debug!("Ebb: Creating native window");
        }
    
        debug!("Ebb: Window creation complete");
        Window::from_raw(window, event_loop)
    }

    /// Runs the window event loop until it closes.
    pub fn run(mut self) {
        let _ = self.evt_loop.run(move |event, control_flow| {
            match event {
                Event::WindowEvent {
                    ref event,
                    window_id,
                } if window_id == self.win.id() => match event {
                    WindowEvent::RedrawRequested => {
                        self.render_fn.as_mut()();
                    },
                    WindowEvent::CloseRequested
                    | WindowEvent::KeyboardInput {
                        event:
                            KeyEvent {
                                state: ElementState::Pressed,
                                physical_key: PhysicalKey::Code(KeyCode::Escape),
                                ..
                            },
                        ..
                    } => control_flow.exit(),
                    WindowEvent::Resized(physical_size) => {
                        self.resize_fn.as_mut()(physical_size);
                    },
                    _ => {}
                },
                Event::AboutToWait => {
                    self.win.request_redraw();
                },
                _ => {}
            }
        });
    }

    /// Set the render callback for the window.
    /// The argument will be called every frame.
    /// 
    /// # Arguments
    /// 
    /// * `renderer` - The render callback subroutine, which will be called every frame.
    pub fn on_render<T: 'a + FnMut()>(&mut self, renderer: T) {
        self.render_fn = Box::new(renderer);
    }

    /// Set the resize callback for the window.
    /// The argument will be called when the window is resized.
    /// 
    /// # Arguments
    /// 
    /// * `cbk` - The resize callback function, with a size argument.
    pub fn on_resize<T:'a + FnMut(&PhysicalSize<u32>)>(&mut self, cbk: T) {
        self.resize_fn = Box::new(cbk);
    }

    /// Get a handle to the internal [winit::window::Window].
    /// 
    /// # Returns
    /// 
    /// An [Arc] containing the internal [winit::window::Window].
    pub fn raw_window(&self) -> Arc<WinitWindow> {
        self.win.clone()
    }
}

/// Boilerplate to create an [ebb::instance::Instance] from a [Window]
#[macro_export]
macro_rules! create_instance {
    ($window:expr) => {{
        let win_clone = $window.raw_window(); // Borrow the raw window
        ebb::instance::Instance::new(win_clone) // Create the instance with the borrowed raw window
    }};
}