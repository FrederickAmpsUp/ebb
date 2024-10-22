use std::rc::Rc;
use glam::*;
use ebb::{self, mesh::Vertex};

#[repr(C)]
#[derive(Clone, Copy)]
struct TestVertex {
    position: Vec2
}
unsafe impl bytemuck::Pod for TestVertex {}
unsafe impl bytemuck::Zeroable for TestVertex {}
impl ebb::mesh::Vertex for TestVertex {
    const LAYOUT: wgpu::VertexBufferLayout<'static> = wgpu::VertexBufferLayout {
        array_stride: std::mem::size_of::<TestVertex>() as wgpu::BufferAddress,
        step_mode: wgpu::VertexStepMode::Vertex,
        attributes: &[
            wgpu::VertexAttribute {
                offset: 0,
                shader_location: 0,
                format: wgpu::VertexFormat::Float32x2,
            },
        ]
    };
}

fn main() {
    let mut world = ebb::ecs::World::new();

    let instance: ebb::Instance<'static>;

    let mut window = ebb::Window::new((800, 600), String::from("Ebb Test - Portals"), false);

    instance = ebb::create_instance!(window);

    let vertices = vec![
        TestVertex { position: vec2(0.0, 0.5) },
        TestVertex { position: vec2(-0.5, -0.5) },
        TestVertex { position: vec2(0.5, -0.5) },
    ];

        // TODO: update pipeline to enable rendering of vertices
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