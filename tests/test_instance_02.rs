use ebb;

fn main() {
    ebb::setup();
    let mut instance: ebb::Instance;

    let mut window = ebb::Window::new((1920, 1080), String::from("Ebb Test Window"), true);
    
    instance = ebb::create_instance!(window);

    window.on_render(|| {

    });

    window.on_resize(|new_size| {
        instance.resize(new_size);
    });

    window.run();
}