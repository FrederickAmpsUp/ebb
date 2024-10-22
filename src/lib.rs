pub mod window;
pub mod instance;
pub mod surface;
pub mod ecs;
pub mod rendering;
pub mod mesh;

pub type Instance<'a> = instance::Instance<'a>;
pub type Window<'a> = window::Window<'a>;

pub fn setup() {
    // can do setup stuff here
}