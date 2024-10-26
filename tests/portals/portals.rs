use std::rc::Rc;
use glam::*;
use ebb;

fn main() {
    let mut engine = ebb::Engine::new((800, 600), String::from("Ebb Test - Portals"));
    
    let pipeline = ebb::rendering::RenderPipeline::for_mesh::<ebb::mesh::PositionVertex3D>(&engine.instance, wgpu::include_wgsl!("assets/shaders/test_model.wgsl"));

    let mesh_renderer = ebb::mesh::RenderMesh::load_obj(&engine.instance, Rc::new(pipeline), "./tests/portals/assets/suzanne.obj").expect("Failed to load mesh.");
    let mut mesh_ent = ebb::ecs::Entity::new();
    mesh_ent.add_component(mesh_renderer);

    engine.world.add_entity(mesh_ent);

    let mut cam = ebb::ecs::Entity::new();
    cam.add_component(ebb::camera::Camera::perspective(engine.window_surface(), 70.0, 0.01, 100.0));
    cam.add_component(ebb::transform::Transform3D {
        model_matrix: Mat4::IDENTITY
    });

    engine.world.add_entity(cam);

    engine.run();
}