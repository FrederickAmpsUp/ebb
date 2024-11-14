use std::rc::Rc;
use glam::*;
use ebb::{self, camera::Camera, transform::Transform3D};

struct CameraMoveSystem {
    start: std::time::Instant
}

impl ebb::ecs::System for CameraMoveSystem {
    fn update(&self, world: &mut ebb::ecs::World) {
        let now = self.start.elapsed().as_secs_f32();
        let origin = glam::vec3(
            f32::cos(now),
            0.0,
            f32::sin(now)
        ) * 3.0;

        let mm = glam::Mat4::look_at_lh(origin, glam::Vec3::ZERO, glam::vec3(0.0,1.0,0.0));

        for ent in world.get_entities_with_all_mut::<(Transform3D, Camera)>() {
            let transform = ent.get_component_mut::<Transform3D>().unwrap();

            transform.model_matrix = mm;
        }
    }
}

fn main() {
    let mut engine = ebb::Engine::new((800, 600), String::from("Ebb Test - Portals"));

    let mut cam = ebb::ecs::Entity::new();
    cam.add_component(ebb::camera::Camera::perspective(engine.window_surface(), 70.0, 0.01, 100.0));
    cam.add_component(ebb::transform::Transform3D {
        model_matrix: Mat4::IDENTITY
    });

    let pipeline = ebb::rendering::RenderPipeline::for_mesh::<ebb::mesh::PositionVertex3D>(&engine.instance, wgpu::include_wgsl!("assets/shaders/test_model.wgsl"));
        // todo better paths
    let mesh_renderer = ebb::mesh::RenderMesh::load_obj(&engine.instance, Rc::new(pipeline), "./tests/portals/assets/box.obj").expect("Failed to load mesh.");
    let mut mesh_ent = ebb::ecs::Entity::new();
    mesh_ent.add_component(mesh_renderer);

    engine.world.add_entity(mesh_ent);
    engine.world.add_entity(cam);

    engine.world.add_system(CameraMoveSystem { start: std::time::Instant::now() });

    engine.run();
}
