use ebb;

fn main() {
    ebb::setup();

    let mut window = ebb::window::Window::new((1920, 1080), String::from("Ebb Test Window"), false);

    window.on_resize(|_new_size| {
        // nothing here :)
    });

    window.on_render(|| {
        // also nothing here :)
    });

    window.run();
}