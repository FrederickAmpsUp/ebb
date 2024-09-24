use ebb;

fn main() {
    ebb::setup();

    let title = String::from("Ebb Test Window");
    let mut window = ebb::window::new((1920, 1080), &title, false);

    window.on_resize(|_new_size| {
        // nothing here :)
    });

    window.on_render(|| {
        // also nothing here :)
    });

    window.run();
}