use ebb;

struct ClearScreen {
    color: wgpu::Color
}
impl ebb::ecs::Component for ClearScreen {}

struct Renderer {
    pub instance: ebb::Instance<'static>
}

impl ebb::ecs::System for Renderer {
    fn update(&self, world: &mut Vec<ebb::ecs::Entity>) {
        for entity in world {
            
                // TODO: more efficient iteration
            if entity.has_component::<ClearScreen>() {
                println!("Clearing screen");
            }
        }
    }
}

fn main() {
    ebb::setup();
    let mut instance: ebb::instance::Instance<'static>;

    let mut world = ebb::ecs::World::new();

    let mut window = ebb::Window::new((1920, 1080), String::from("Ebb Test Window"), false);
    
    instance = ebb::create_instance!(window);


    let clear = ClearScreen { color: wgpu::Color::RED };
    let mut clear_ent = ebb::ecs::Entity::new();
    clear_ent.add_component(clear);

    world.add_entity(clear_ent);
    world.add_system(Renderer { instance });

    window.on_render(|| {
        world.update();
    });

    window.on_resize(|new_size| {
            // TODO: figure out how to manage the borrow here
        //instance.resize(new_size);
    });

    window.run();
}