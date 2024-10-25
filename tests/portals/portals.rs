use std::rc::Rc;
use bytemuck::{Pod, Zeroable};
use glam::*;
use ebb;

#[repr(C)]
#[derive(Clone, Copy)]
struct TestVertex {
    position: Vec2,
    color: Vec3
}

// TODO: get the procedural macro for attribute generation working again
impl ebb::mesh::Vertex for TestVertex {
    const LAYOUT: wgpu::VertexBufferLayout<'static> = wgpu::VertexBufferLayout {
        array_stride: size_of::<TestVertex>() as u64,
        step_mode: wgpu::VertexStepMode::Vertex,
        attributes: &wgpu::vertex_attr_array![0 => Float32x2, 1 => Float32x3],
    };
}

unsafe impl Pod for TestVertex {}
unsafe impl Zeroable for TestVertex {}

fn main() {
    let mut engine = ebb::Engine::new((800, 600), String::from("Ebb Test - Portals"));
    
    let pipeline = ebb::rendering::RenderPipeline::for_mesh::<TestVertex>(&engine.instance, wgpu::include_wgsl!("assets/shaders/test_triangle.wgsl"));

    let vertices: Vec<TestVertex> = vec![
        TestVertex { position: vec2(0.0, 0.5), color: vec3(1.0, 0.0, 0.0) },
        TestVertex { position: vec2(-0.5, -0.5), color: vec3(0.0, 1.0, 0.0) },
        TestVertex { position: vec2(0.5, -0.5), color: vec3(0.0, 0.0, 1.0) },
    ];

    let indices: Vec<u32> = vec![
        0, 1, 2
    ];

    let test_triangle = ebb::mesh::RenderMesh::entity(&engine.instance, Rc::new(pipeline), &vertices, &indices);

    engine.world.add_entity(test_triangle);

    engine.run();
}