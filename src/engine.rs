use crate as ebb;

pub struct Engine {
    pub window: ebb::Window<'static>,
    pub world: ebb::ecs::World,
    pub instance: ebb::Instance<'static>
}

impl Engine {
    pub fn new(win_size: (u32, u32), win_title: String) -> Self {
        let window = ebb::Window::new(win_size, win_title, false);
        let instance = ebb::create_instance!(window);

        Self {
            window,
            world: ebb::ecs::World::new(),
            instance
        }
    }

    pub fn run(mut self) {
        self.world.add_system(ebb::rendering::BasicRenderSystem::new(self.instance, wgpu::Color::BLACK));
        
        self.window.on_render(move || {
           self.world.update(); 
        });

        self.window.run();
    }
}