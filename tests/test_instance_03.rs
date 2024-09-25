use ebb;
use wasm_bindgen::prelude::*;

#[async_std::main]
#[wasm_bindgen(start)]
pub async fn main() {
    ebb::setup();

    let title = String::from("Ebb Test Window");
    let mut window = ebb::window::new((1920, 1080), &title, false);

    let instance = ebb::instance::Instance::new(&window).await;

    let queue = instance.raw_queue();

    instance.window().on_render(|| {

    });
}