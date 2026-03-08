#include "imgui.h"
#include <sokol_app.h>
#include <sokol_gfx.h>

#if !defined(SOKOL_GLCORE) && !defined(SOKOL_GLES3) && !defined(SOKOL_D3D11) && !defined(SOKOL_METAL) && !defined(SOKOL_WGPU)
    #if defined(_WIN32)
        #define SOKOL_D3D11
    #elif defined(__APPLE__)
        #define SOKOL_METAL
    #else
        #define SOKOL_GLCORE
    #endif
#endif

#define SOKOL_IMGUI_IMPL
#include <util/sokol_imgui.h>
