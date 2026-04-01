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

#if IMGUI_VERSION_NUM < 19100
// sokol_imgui expects newer clipboard hooks on ImGuiPlatformIO.
#define ImGuiContext void
#define ImGuiPlatformIO ImGuiIO
#define GetPlatformIO GetIO
#define Platform_SetClipboardTextFn SetClipboardTextFn
#define Platform_GetClipboardTextFn GetClipboardTextFn
#endif

#define SOKOL_IMGUI_IMPL
#include <util/sokol_imgui.h>

#if IMGUI_VERSION_NUM < 19100
#undef Platform_GetClipboardTextFn
#undef Platform_SetClipboardTextFn
#undef GetPlatformIO
#undef ImGuiPlatformIO
#undef ImGuiContext
#endif
