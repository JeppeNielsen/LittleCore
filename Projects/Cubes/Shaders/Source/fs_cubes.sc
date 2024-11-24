$input v_color0, vertexPosition

/*
 * Copyright 2011-2023 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx/blob/master/LICENSE
 */

#include "../../../External/bgfx/examples/common/common.sh"

void main()
{
	gl_FragColor = vec4(v_color0.r + vertexPosition.y, v_color0.g, v_color0.b, 0);
}
