//
// Created by Jeppe Nielsen on 02/12/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "MaxRectsPacker.hpp"

namespace LittleCore {
    class FontAtlas {
    public:

        struct GlyphBounds {
            double l, b, r, t;
        };

        struct GlyphInfo {
            uint16_t x = 0, y = 0, width = 0, height = 0;
            float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
            float quadWidth = 0;
            float quadHeight = 0;

            float advance = 0.0f;
            GlyphBounds bounds;
            GlyphBounds atlasBounds;

            bool valid = false;
        };

        struct Params {
            uint16_t initialWidth = 512;
            uint16_t initialHeight = 512;
            uint16_t padding = 2;

            bgfx::TextureFormat::Enum format = bgfx::TextureFormat::RGBA8;
            uint64_t flags =
                    BGFX_TEXTURE_NONE |
                    BGFX_SAMPLER_U_CLAMP |
                    BGFX_SAMPLER_V_CLAMP;

            bool keepCpuCopy = true;
            const char* debugName = "FontAtlas";
            uint16_t maxSize = 4096;
        };

        FontAtlas(const Params& params);

        ~FontAtlas();

        void reset(uint16_t newWidth, uint16_t newHeight);

        bgfx::TextureHandle texture() const;

        uint16_t width() const;

        uint16_t height() const;

        const GlyphInfo* find(uint32_t glyphId) const;

        const GlyphInfo* addGlyph(
                uint32_t glyphId,
                const uint8_t* pixels,
                uint16_t glyphWidth,
                uint16_t glyphHeight,
                float advance,
                const GlyphBounds& bounds,
                const GlyphBounds& atlasBounds
        );

    private:
        Params params;
        bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

        uint16_t atlasWidth = 0, atlasHeight = 0;
        uint8_t bytesPerPixel = 4;

        std::vector<uint8_t> cpuPixels;
        std::unordered_map<uint32_t, GlyphInfo> glyphs;
        MaxRectsPacker packer;

    private:
        static uint8_t bytesPerPixelFor(bgfx::TextureFormat::Enum format);

        void destroyTexture();

        void computeUVs(GlyphInfo& info) const;

        bool growToFit(uint16_t neededWidth, uint16_t neededHeight);

        void rebuildTexture(uint16_t newWidth, uint16_t newHeight);

        void blitToCpu(uint16_t destX, uint16_t destY,
                       uint16_t glyphWidth, uint16_t glyphHeight,
                       const uint8_t* pixels);

        void uploadRegion(uint16_t destX, uint16_t destY,
                          uint16_t glyphWidth, uint16_t glyphHeight,
                          const uint8_t* pixels);
    };
}