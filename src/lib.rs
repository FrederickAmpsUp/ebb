pub mod window;
pub mod instance;
pub mod surface;
pub mod ecs;
pub mod rendering;
pub mod mesh;
pub mod engine;
pub mod camera;
pub mod transform;

/*
 * TODO:
 * - add more commonly used types below
*/

pub type Instance<'a> = instance::Instance<'a>;
pub type Window<'a> = window::Window<'a>;
pub type Engine = engine::Engine;

pub fn setup() {
    // can do setup stuff here (there's nothing)
}