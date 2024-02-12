{

    vec4 v_color0    : COLOR0    = vec4(1.0, 0.0, 0.0, 1.0);
    vec3 vertexPosition : POSITION;
    vec3 a_position  : POSITION;
    vec4 a_color0    : COLOR0;

}

{

    $input a_position, a_color0
    $output v_color0, vertexPosition

    #include "../../../External/bgfx/examples/common/common.sh"

    vec2 Not() {
        return 0;
    }

    vec3 GetNormal() {
        return Not();
    }

    void main()
    {
    	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
    	gl_Position.x += sin(a_position.x+a_position.y);
    	v_color0 = a_color0;
    	vertexPosition = gl_Position;
    }

}

{

    $input v_color0, vertexPosition

    #include "../../../External/bgfx/examples/common/common.sh"

    void main()
    {
        gl_FragColor = vec4(v_color0.r + vertexPosition.y, v_color0.g, v_color0.b, 0);
    }

}


