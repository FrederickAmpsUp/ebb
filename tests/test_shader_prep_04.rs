use ebb;

fn main() {
    let shader = ebb::rendering::shader::load_shader("tests/assets/test_04.wgsl");

    if let Err(ref e) = shader {
        println!("Shader error: {:?}", e);
    }
}
