use std::rc::Rc;

use ebb;

fn main() {
    let mut world = ebb::ecs::World::new();

    let instance: ebb::Instance<'static>;

    let mut window = ebb::Window::new((800, 600), String::from("Ebb Test - Portals"), false);

    instance = ebb::create_instance!(window);

    let pipeline = ebb::rendering::RenderPipeline::new(&instance, wgpu::include_wgsl!("assets/shaders/test_triangle_nobuf.wgsl"));
    let test_triangle = ebb::rendering::RenderMesh::entity(Rc::new(pipeline));

    world.add_entity(test_triangle);
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