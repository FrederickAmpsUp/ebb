use crate as ebb;

/// A wrapper over the state of an app, including a window, instance, and an [ebb::ecs::World].
pub struct Engine {
    pub window: ebb::Window<'static>,
    pub world: ebb::ecs::World,
    pub instance: ebb::Instance<'static>
}

impl Engine {
    /// Creates a new engine/application.
    /// 
    /// # Arguments
    /// 
    /// * `win_size` - The size of the window, in pixels.
    /// * `win_title` - The title of the window.
    /// 
    /// # Returns
    /// 
    /// The engine.
    pub fn new(win_size: (u32, u32), win_title: String) -> Self {
        let window = ebb::Window::new(win_size, win_title, false);
        let instance = ebb::create_instance!(window);

        Self {
            window,
            world: ebb::ecs::World::new(),
            instance
        }
    }

    /// Run the application until the window is closed.
    /// This currently uses the [ebb::rendering::BasicRenderSystem]
    pub fn run(mut self) {
        // TODO: come up with a clean way to use different render systems.

        self.world.add_system(ebb::rendering::BasicRenderSystem::new(self.instance, wgpu::Color::BLACK));
        
        self.window.on_render(move || {
           self.world.update(); 
        });

        self.window.run();
    }
}