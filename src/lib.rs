pub mod window;
pub mod instance;
pub mod surface;
pub mod rendering_context;
pub mod ecs;

pub type Instance<'a> = instance::Instance<'a>;
pub type Window<'a> = window::Window<'a>;

pub fn setup() {
    // can do setup stuff here
}