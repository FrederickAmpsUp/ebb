use ebb;

fn main() {
    let shader = ebb::rendering::shader::load_shader("tests/portals/assets/shaders/test_model.wgsl");

    match shader {
        Err(e) =>
            println!("Shader error: {:?}", e),
        Ok(s) => {
            println!("{:?}", s.uniforms());
        }
    }

}
