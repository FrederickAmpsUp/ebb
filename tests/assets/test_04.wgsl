var<uniform> my_uniform: vec3<f32>;

// random bs
fn test_fn(a: f32, b: vec2<f32>) -> u32 {
    if (a < b[a + bi].r) {
        return 1;
    } else {
        return 0;
    }
}

struct TestStructForAlignmentAndPadding {
    a: vec3<f32>, // 16 bytes
    b: vec2<i32>, // 8 bytes
    c: vec4<u32>, // 16 bytes
    d: f32, // 4 bytes
}

var<uniform> test_uniform: TestStructForAlignmentAndPadding;

var test_var: vec2<f32> = (1 + 2) * 3;