use std::rc::Rc;
use glam::*;
use ebb;
use ebb_macros::Vertex;

#[repr(C)]
#[derive(Clone, Copy, Vertex)]
struct TestVertex {
    position: Vec2,
    color: Vec3
}

fn main() {
    let mut engine = ebb::Engine::new((800, 600), String::from("Ebb Test - Portals"));

    let vertices: Vec<TestVertex> = vec![
        TestVertex { position: vec2(0.0, 0.5), color: vec3(1.0, 0.0, 0.0) },
        TestVertex { position: vec2(-0.5, -0.5), color: vec3(0.0, 1.0, 0.0) },
        TestVertex { position: vec2(0.5, -0.5), color: vec3(0.0, 0.0, 1.0) },
    ];

    let indices: Vec<u32> = vec![
        0, 1, 2
    ];

    let pipeline = ebb::rendering::RenderPipeline::for_mesh::<TestVertex>(&engine.instance, wgpu::include_wgsl!("assets/shaders/test_triangle.wgsl"));
    let test_triangle = ebb::mesh::RenderMesh::entity(&engine.instance, Rc::new(pipeline), vertices, indices);

    engine.world.add_entity(test_triangle);

    engine.run();
}