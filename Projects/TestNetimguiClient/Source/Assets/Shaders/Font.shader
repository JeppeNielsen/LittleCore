@vs vs
layout(binding=0) uniform vs_params {
    mat4 u_modelViewProj;
};

layout(location=0) in vec3 a_position;
layout(location=1) in vec4 a_color0;
layout(location=2) in vec2 a_texcoord0;

layout(location=0) out vec2 v_texcoord0;

void main() {
    gl_Position = u_modelViewProj * vec4(a_position, 1.0);
    v_texcoord0 = a_texcoord0;
}
@end

@fs fs
layout(binding=0) uniform texture2D colorTexture;
layout(binding=0) uniform sampler colorTextureSampler;

layout(binding=1) uniform fs_params {
    vec4 outlineSize;
    vec4 color;
    vec4 outlineColor;
};

layout(location=0) in vec2 v_texcoord0;
layout(location=0) out vec4 frag_color;

float screenPxRange(vec2 pos) {
    vec2 unitRange = vec2(8.0) / vec2(1024.0, 1024.0);
    vec2 screenTexSize = vec2(1.0) / fwidth(pos);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 sdf = texture(sampler2D(colorTexture, colorTextureSampler), v_texcoord0).rgb;
    float sd = sdf.r;

    float pxRange = screenPxRange(v_texcoord0);
    float screenPxDist = pxRange * (sd - 0.49);
    float alpha = clamp(screenPxDist + 0.5, 0.0, 1.0);

    float outlinePx = pxRange * (sd - 0.49 - outlineSize.x);
    float outlineAmount = clamp(outlinePx + 0.5, 0.0, 1.0);
    vec4 col = mix(outlineColor, color, outlineAmount);

    frag_color = vec4(col.rgb, alpha * col.a * color.a);
}
@end

@program TestShader vs fs

