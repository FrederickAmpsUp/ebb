use ebb;

fn main() {
    let mut world = ebb::ecs::World::new();

    let instance: ebb::Instance<'static>;

    let mut window = ebb::Window::new((800, 600), String::from("Ebb Test - Portals"), false);

    instance = ebb::create_instance!(window);

    world.add_system(ebb::rendering::BasicRenderSystem::new(instance, wgpu::Color { r: 1.0, g: 0.0, b: 1.0, a: 1.0 }));

    /*
        // TODO: figure out the freaking resizing thing (borrow checker)
    window.on_resize(|new_size| {
        instance.resize(new_size);
    });*/

    window.on_render(|| {
        world.update();
    });

    window.run();
}