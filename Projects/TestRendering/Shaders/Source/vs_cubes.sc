$input a_position, a_color0
$output v_color0

/*
 * Copyright 2011-2023 Branimir Karadzic. All rights reserved.
 * Legacy shader source license: see upstream project documentation.
 */
// common include removed during sokol migration
void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );
	v_color0 = a_color0;
}
