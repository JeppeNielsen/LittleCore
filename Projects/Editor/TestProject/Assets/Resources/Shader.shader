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

    #include "../../../../../External/bgfx/examples/common/common.sh"

    void main()
    {
        //a_position.x += sin(a_position.x+a_position.y*3);
    	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    	v_color0 = a_color0;
    	v_texcoord0 = a_texcoord0;
    	vertexPosition = gl_Position;
    }

}

{

    $input v_color0, vertexPosition, v_texcoord0

    #include "../../../../../External/bgfx/examples/common/common.sh"
    SAMPLER2D(colorTexture,  0);

    void main()
    {
        vec4 mColor = texture2D(colorTexture,v_texcoord0);
        gl_FragColor = vec4(1,1,0,1);// vec4(mColor.rgb, 1);
    }

}


