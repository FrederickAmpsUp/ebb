use std::rc::Rc;
use glam::*;
use ebb::{self, mesh::Vertex};
use ebb_macros::Vertex;

#[repr(C)]
#[derive(Clone, Copy, Vertex)]
struct TestVertex {
    position: Vec2
}

fn main() {
    let mut world = ebb::ecs::World::new();

    let instance: ebb::Instance<'static>;

    let mut window = ebb::Window::new((800, 600), String::from("Ebb Test - Portals"), false);

    instance = ebb::create_instance!(window);

    let vertices: Vec<TestVertex> = vec![
        TestVertex { position: vec2(0.0, 0.5) },
        TestVertex { position: vec2(-0.5, -0.5) },
        TestVertex { position: vec2(0.5, -0.5) },
    ];

    let pipeline = ebb::rendering::RenderPipeline::new(&instance, &[TestVertex::LAYOUT], wgpu::include_wgsl!("assets/shaders/test_triangle.wgsl"));
    let test_triangle = ebb::mesh::RenderMesh::entity(&instance, Rc::new(pipeline), vertices);

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