#pragma once
#include <bgfx/bgfx.h>

class TextureSaver
{
public:
    // Save a texture handle to a .tga file.
    // Supports RGBA8 and BGRA8 directly. Other formats are blitted to BGRA8.
    // Only works for 2D, single-layer, non-cubemap textures.
    static bool SaveTGA(bgfx::TextureHandle handle,
                        const char* path,
                        uint16_t w, uint16_t h,
                        bgfx::TextureFormat::Enum format,
                        bool withAlpha);
};
