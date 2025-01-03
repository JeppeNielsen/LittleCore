$input a_position, a_color0
$output v_color0, vertexPosition

/*
 * Copyright 2011-2023 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../../../External/bgfx/examples/common/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	gl_Position.x += sin(a_position.x+a_position.y);
	v_color0 = a_color0;
	vertexPosition = gl_Position;
}
