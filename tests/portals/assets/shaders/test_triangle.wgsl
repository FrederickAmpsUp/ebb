struct VertexInput {
    @location(0) position: vec2<f32>
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) vert_pos: vec2<f32>
};

@vertex
fn vs_main(
    in_vert: VertexInput,
) -> VertexOutput {
    var out: VertexOutput;
    out.clip_position = vec4<f32>(in_vert.position, 0.0, 1.0);
    out.vert_pos = out.clip_position.xy;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(0.5 + in.vert_pos, 0.0, 1.0);
}