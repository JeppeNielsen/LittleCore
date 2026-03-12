@vs vs
layout(binding=0) uniform vs_params {
    mat4 u_modelViewProj;
};

layout(location=0) in vec3 a_position;
layout(location=1) in vec4 a_color0;
layout(location=2) in vec2 a_texcoord0;

layout(location=0) out vec4 v_color0;
layout(location=1) out vec2 v_texcoord0;
layout(location=2) out vec2 v_pos;

void main() {
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
    v_color0 = a_color0;
    v_texcoord0 = a_texcoord0;
    v_pos = gl_Position.xy;
}
@end

@fs fs
layout(binding=0) uniform texture2D colorTexture;
layout(binding=0) uniform sampler colorTextureSampler;

layout(location=0) in vec4 v_color0;
layout(location=1) in vec2 v_texcoord0;
layout(location=2) in vec2 v_pos;

layout(location=0) out vec4 frag_color;

void main() {
    vec4 mColor = texture(sampler2D(colorTexture, colorTextureSampler), v_texcoord0 + vec2(v_pos.x,0));
    frag_color = vec4(mColor.rgb * v_color0.rgb, 1.0);
}
@end

@program TestShader vs fs

