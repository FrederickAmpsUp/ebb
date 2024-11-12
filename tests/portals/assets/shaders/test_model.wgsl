struct VertexInput {
    @location(0) position: vec3<f32>,
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) vert_pos: vec3<f32>,
};

struct EbbBuiltinUniforms {
    proj_mtx: mat4x4<f32>,
    view_mtx: mat4x4<f32>
}

@group(0) @binding(0)
var<uniform> ebb: EbbBuiltinUniforms;

@vertex
fn vs_main(
    in_vert: VertexInput,
) -> VertexOutput {
    var out: VertexOutput;
    out.clip_position = ebb.proj_mtx * ebb.view_mtx * vec4<f32>(in_vert.position, 1.0);
    out.vert_pos = in_vert.position;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(in.vert_pos, 1.0);
}