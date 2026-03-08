$input v_color0

/*
 * Copyright 2011-2023 Branimir Karadzic. All rights reserved.
 * Legacy shader source license: see upstream project documentation.
 */
// common include removed during sokol migration
void main()
{
	gl_FragColor = vec4(v_color0.r, 0,0,0);
}
