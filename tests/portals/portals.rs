use ebb;

struct TestRendering {

}

impl TestRendering {
    fn entity() -> ebb::ecs::Entity {
        let mut entity = ebb::ecs::Entity::new();
        entity.add_component(TestRendering {});
        entity
    }
}
impl ebb::ecs::Component for TestRendering {}

struct TestRenderingSystem {
    instance: ebb::Instance<'static>
}

impl ebb::ecs::System for TestRenderingSystem {
    fn update(&self, world: &mut Vec<ebb::ecs::Entity>) {
        let output = self.instance.window_surface().wgpu_surface().get_current_texture().expect("Failed to acquire window surface texture");
        let view = output.texture.create_view(&wgpu::TextureViewDescriptor::default());

        let mut render_ctx = ebb::rendering::RenderContext::new(&self.instance);

        render_ctx.clear(&view, wgpu::Color { r: 1.0, g: 0.0, b: 1.0, a: 1.0 });

        for entity in world {
            if entity.has_component::<TestRendering>() {
                // TODO: render
            }
        }

        render_ctx.submit(&self.instance);
        output.present();
    }
}

fn main() {
    let mut world = ebb::ecs::World::new();

    let render_tester = TestRendering::entity();

    world.add_entity(render_tester);

    let instance: ebb::Instance<'static>;

    let mut window = ebb::Window::new((800, 600), String::from("Ebb Test - Portals"), false);

    instance = ebb::create_instance!(window);

    world.add_system(TestRenderingSystem { instance });

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