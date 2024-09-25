use ebb;
use wasm_bindgen::prelude::*;

#[async_std::main]
#[wasm_bindgen(start)]
pub async fn main() {
    ebb::setup();

    let title = String::from("Ebb Test Window");
    let mut window = ebb::window::new((800, 600), &title, false);

    let instance = ebb::instance::Instance::new(&mut window).await;

    let queue = instance.raw_queue();

    instance.window_mut().on_render(|| {

    });
}