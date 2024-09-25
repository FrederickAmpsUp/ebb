use winit::{
    event_loop::EventLoop,
    window::WindowBuilder,
    window::Window as WinitWindow,
    dpi::PhysicalSize,
    event::*,
    keyboard::*
};

use log::*;

use std::rc::Rc;

pub struct Window<'a> {
    win: Rc<WinitWindow>,
    evt_loop: EventLoop<()>,

    render_fn: Box<dyn FnMut() + 'a>,
    resize_fn: Box<dyn FnMut(&PhysicalSize<u32>) + 'a>
}

impl<'a> Window<'a> {
    /**
     * Creates a new window from the raw Winit fields
     * @param win The window
     * @param evt_loop The event loop
     * @return The VoxelForge-wrapped window
     */
    pub fn raw(win: WinitWindow, evt_loop: EventLoop<()>) -> Self {
        Self {
            win: Rc::new(win),
            evt_loop,
            render_fn: Box::new(|| {}),
            resize_fn: Box::new(|_| {}),
        }
    }

    /**
     * Creates a new window.
     * This will use the ebb-canvas element on web platforms.
     * @param size The size of the window
     * @param title The title of the window
     * @return The window
     */
    pub fn new(size: (u32, u32), title: &String, resizable: bool) -> Window {
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
        Window::raw(window, event_loop)
    }

    /**
     * Run the window event loop until it closes
     */
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

    pub fn on_render(&mut self, renderer: impl 'a + FnMut()) {
        self.render_fn = Box::new(renderer);
    }

    pub fn on_resize(&mut self, cbk: impl 'a + FnMut(&PhysicalSize<u32>)) {
        self.resize_fn = Box::new(cbk);
    }

    pub fn raw_window(&self) -> &Rc<WinitWindow> {
        &self.win
    }
}

/**
 * Same as Window::new
 */
pub fn new(preferred_size: (u32, u32), preferred_title: &String, resizable: bool) -> Window {
    return Window::new(preferred_size, preferred_title, resizable);
}