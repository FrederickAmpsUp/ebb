use ebb;

fn main() {
    ebb::setup();
    let mut instance: ebb::instance::Instance;

    let mut window = ebb::window::Window::new((1920, 1080), String::from("Ebb Test Window"), true);
    
    instance = ebb::create_instance!(window);

    window.on_render(|| {

    });

    window.on_resize(|new_size| {
        instance.resize(new_size);
    });

    window.run();
}