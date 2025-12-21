{

    vec4 v_color0    : COLOR0    = vec4(1.0, 0.0, 0.0, 1.0);
    vec3 vertexPosition : POSITION;
    vec2 v_texcoord0 : TEXCOORD0;
    vec3 a_position  : POSITION;
    vec4 a_color0    : COLOR0;
    vec2 a_texcoord0 : TEXCOORD0;

}

{

    $input a_position, a_color0, a_texcoord0
    $output v_color0, vertexPosition, v_texcoord0

    #include "../../../../External/bgfx/examples/common/common.sh"

    void main()
    {
        gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    	v_color0 = a_color0;
    	v_texcoord0 = a_texcoord0;// + vec2(1.0) / vec2(512.0);
    	vertexPosition = gl_Position;
    }

}

{

    $input v_color0, vertexPosition, v_texcoord0

    #include "../../../../External/bgfx/examples/common/common.sh"
    SAMPLER2D(colorTexture,  0);
    uniform vec4 outlineSize;
	uniform vec4 color;
	uniform vec4 outlineColor;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 pos) {
    vec2 unitRange = vec2(8)/vec2(1024,1024);
    vec2 screenTexSize = vec2(1.0f)/fwidth(pos);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}


    void main()
    {
	vec3 sdf = texture2D(colorTexture, v_texcoord0).rgb;

    float sd = sdf.r; //median(sdf.r, sdf.g, sdf.b);


    float pxRange = screenPxRange(v_texcoord0);
    float screenPxDist = pxRange*(sd - 0.49);
    float alpha = clamp(screenPxDist + 0.5, 0.0, 1.0);


    float outlinePx = pxRange*(sd - 0.49 - outlineSize.x);
    float outlineAmount = clamp(outlinePx + 0.5, 0.0, 1.0);

	vec4 col = lerp(outlineColor, color, outlineAmount);


    gl_FragColor = vec4(col.r, col.g, col.b, alpha * col.a * color.a);
    }

}


