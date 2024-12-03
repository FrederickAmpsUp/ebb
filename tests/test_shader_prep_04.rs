use ebb;

fn main() {
    let shader = ebb::rendering::shader::load_shader("tests/assets/test_04.wgsl");

    match shader {
        Err(e) =>
            println!("Shader error: {:?}", e),
        Ok(s) => {
            println!("{:?}", s.uniforms());
        }
    }

}
