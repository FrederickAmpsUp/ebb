struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) vert_pos: vec2<f32>
};

@vertex
fn vs_main(
    @builtin(vertex_index) in_vertex_index: u32,
) -> VertexOutput {
    var vertices: array<vec2<f32>, 3> = array<vec2<f32>, 3>(
        vec2<f32>(0.5, -0.5),
        vec2<f32>(0.0, 0.5),
        vec2<f32>(-0.5, -0.5)
    );

    var out: VertexOutput;
    out.clip_position = vec4<f32>(vertices[in_vertex_index], 0.0, 1.0);
    out.vert_pos = out.clip_position.xy;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(0.5 + in.vert_pos, 0.0, 1.0);
}