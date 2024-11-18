struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) texcoord: vec2<f32>
}

struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) vert_pos: vec3<f32>,
    @location(1) vert_texcoord: vec2<f32>
};

struct EbbBuiltinUniforms {
    proj_mtx: mat4x4<f32>,
    view_mtx: mat4x4<f32>
}

@group(0) @binding(0)
var<uniform> ebb: EbbBuiltinUniforms;

@group(1) @binding(0)
var tex: texture_2d<f32>;
@group(1) @binding(1)
var samp: sampler;

@vertex
fn vs_main(
    in_vert: VertexInput,
) -> VertexOutput {
    var out: VertexOutput;
    out.clip_position = ebb.proj_mtx * ebb.view_mtx * vec4<f32>(in_vert.position, 1.0);
    out.vert_pos = in_vert.position;
    out.vert_texcoord = in_vert.texcoord;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
	let abs_sin = abs(sin(10.0*in.vert_pos));

	let quant = step(abs_sin, vec3<f32>(0.5));

	let texCol = textureSample(tex, samp, vec2<f32>(in.vert_texcoord.x, 1.0 - in.vert_texcoord.y));

	var col = texCol;

	return col;
}
