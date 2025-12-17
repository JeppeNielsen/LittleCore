#pragma once
#include <bgfx/bgfx.h>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

#include "MaxRectsPacker.hpp"

struct stbtt_fontinfo;

namespace LittleCore {
    class FontAtlas {
    public:
        struct Config {
            uint16_t pageW = 1024;
            uint16_t pageH = 1024;

            // stbtt_GetCodepointSDF params
            int sdfPadding = 8;
            unsigned char onEdgeValue = 128;
            float pixelDistScale = 32.0f;

            // Gap around glyph rects in the atlas to avoid bleeding.
            uint16_t packingPadding = 1;
        };

        struct GlyphQuad {
            float x0, y0, x1, y1; // screen-space pixels (y-down), penY is baseline
            float u0, v0, u1, v1; // UVs
            float advanceX;       // pen advance in pixels
            uint16_t pageIndex;   // which atlas page/texture to sample
        };

        FontAtlas();

        ~FontAtlas();

        FontAtlas(const FontAtlas&) = delete;

        FontAtlas& operator=(const FontAtlas&) = delete;

        bool initFromFile(const char* path, float pixelHeight, const Config& cfg);

        bool initFromMemory(const void* ttfData, size_t ttfSize, float pixelHeight, const Config& cfg);

        void destroy();

        float pixelHeight() const;

        uint32_t pageCount() const;

        bgfx::TextureHandle pageTexture(uint16_t pageIndex) const;

        bool ensureGlyph(uint32_t codepoint);

        bool getQuad(uint32_t codepoint, float& penX, float& penY, GlyphQuad& out);

        static std::vector<uint32_t> utf8ToCodepoints(const std::string& s);

    private:
        struct Page;
        struct Glyph;

        bool createPage();

        bool rasterizeAndPackGlyph(uint32_t codepoint);

    private:
        Config m_cfg{};

        // Keep bytes alive (safe default when loading from file)
        std::vector<uint8_t> m_ttfOwned;

        // stbtt_fontinfo points into this buffer (owned or external)
        const unsigned char* m_ttf = nullptr;
        size_t m_ttfSize = 0;

        stbtt_fontinfo* m_font = nullptr;

        float m_pixelHeight = 0.0f;
        float m_scale = 0.0f;

        std::vector<Page> m_pages;
        std::unordered_map<uint32_t, Glyph> m_glyphs;
    };
}