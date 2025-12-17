#include "SDFFontAtlasDynamic.hpp"

#include <algorithm>
#include <cstdio>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "ImageLoader.hpp"

struct SdfFontAtlasDynamic::Page {
    uint16_t w = 0, h = 0;
    bgfx::TextureHandle tex = BGFX_INVALID_HANDLE;

    std::vector<uint8_t> bitmap; // CPU-side R8, for reference/debugging

    LittleCore::MaxRectsPacker packer;
};

struct SdfFontAtlasDynamic::Glyph {
    uint16_t pageIndex = 0;
    uint16_t x = 0, y = 0, w = 0, h = 0;

    int16_t xoff = 0;
    int16_t yoff = 0;

    float advanceX = 0.0f;

    float u0 = 0, v0 = 0, u1 = 0, v1 = 0;
};

static bool readFileBytes(const char* path, std::vector<uint8_t>& out) {
    out.clear();
    if (!path) return false;

    FILE* f = std::fopen(path, "rb");
    if (!f) return false;

    std::fseek(f, 0, SEEK_END);
    long len = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);

    if (len <= 0) {
        std::fclose(f);
        return false;
    }

    out.resize((size_t) len);
    size_t n = std::fread(out.data(), 1, (size_t) len, f);
    std::fclose(f);

    if (n != (size_t) len) {
        out.clear();
        return false;
    }
    return true;
}

SdfFontAtlasDynamic::SdfFontAtlasDynamic() = default;

SdfFontAtlasDynamic::~SdfFontAtlasDynamic() {
    destroy();
}

bool SdfFontAtlasDynamic::initFromFile(const char* path, float pixelHeight, const Config& cfg) {
    destroy();

    if (!readFileBytes(path, m_ttfOwned))
        return false;

    return initFromMemory(m_ttfOwned.data(), m_ttfOwned.size(), pixelHeight, cfg);
}

bool SdfFontAtlasDynamic::initFromMemory(const void* ttfData, size_t ttfSize, float pixelHeight, const Config& cfg) {
    destroy();

    if (!ttfData || ttfSize == 0 || pixelHeight <= 0.0f)
        return false;

    m_cfg = cfg;
    m_ttf = (const unsigned char*) ttfData;
    m_ttfSize = ttfSize;

    m_font = new stbtt_fontinfo{};
    if (!stbtt_InitFont(m_font, m_ttf, 0)) {
        destroy();
        return false;
    }

    m_pixelHeight = pixelHeight;
    m_scale = stbtt_ScaleForPixelHeight(m_font, m_pixelHeight);

    return createPage();
}

void SdfFontAtlasDynamic::destroy() {
    for (auto& p: m_pages) {
        if (bgfx::isValid(p.tex))
            bgfx::destroy(p.tex);
        p.tex = BGFX_INVALID_HANDLE;
    }

    m_pages.clear();
    m_glyphs.clear();

    delete m_font;
    m_font = nullptr;

    m_ttfOwned.clear();
    m_ttf = nullptr;
    m_ttfSize = 0;

    m_pixelHeight = 0.0f;
    m_scale = 0.0f;
}

float SdfFontAtlasDynamic::pixelHeight() const { return m_pixelHeight; }

uint32_t SdfFontAtlasDynamic::pageCount() const { return (uint32_t)m_pages.size(); }

bgfx::TextureHandle SdfFontAtlasDynamic::pageTexture(uint16_t pageIndex) const {
    return (pageIndex < m_pages.size()) ? m_pages[pageIndex].tex : (bgfx::TextureHandle) BGFX_INVALID_HANDLE;
}

bool SdfFontAtlasDynamic::ensureGlyph(uint32_t codepoint) {
    return (m_glyphs.find(codepoint) != m_glyphs.end()) ? true : rasterizeAndPackGlyph(codepoint);
}

static bool isWhitespace(uint32_t cp) {
    return cp == 0x20u  // space
           || cp == 0x09u  // tab
           || cp == 0x0Au  // \n
           || cp == 0x0Du; // \r
}

bool SdfFontAtlasDynamic::getQuad(uint32_t codepoint, float& penX, float& penY, GlyphQuad& out) {
    /*if (isWhitespace(codepoint)) {
        penX += m_pixelHeight * m_scale;
        return false;
    }
     */

    if (!ensureGlyph(codepoint))
        return false;

    const Glyph& g = m_glyphs[codepoint];

    if (isWhitespace(codepoint)) {
        penX += g.advanceX;
        return false;
    }

    const float x0 = penX + (float) g.xoff;
    const float y0 = penY + (float) g.yoff;
    const float x1 = x0 + (float) g.w;
    const float y1 = y0 + (float) g.h;

    out.x0 = x0;
    out.y0 = y0;
    out.x1 = x1;
    out.y1 = y1;
    out.u0 = g.u0;
    out.v0 = g.v0;
    out.u1 = g.u1;
    out.v1 = g.v1;
    out.advanceX = g.advanceX;
    out.pageIndex = g.pageIndex;

    penX += g.advanceX;
    return true;
}

std::vector<uint32_t> SdfFontAtlasDynamic::utf8ToCodepoints(const std::string& s) {
    std::vector<uint32_t> cps;
    cps.reserve(s.size());

    const uint8_t* p = (const uint8_t*) s.data();
    const uint8_t* e = p + s.size();

    while (p < e) {
        uint32_t cp = 0xFFFD;
        uint8_t c0 = *p++;

        if (c0 < 0x80) cp = c0;
        else if ((c0 >> 5) == 0x6 && p < e) {
            uint8_t c1 = *p++;
            if ((c1 & 0xC0) == 0x80) cp = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
        } else if ((c0 >> 4) == 0xE && (p + 1) < e) {
            uint8_t c1 = *p++;
            uint8_t c2 = *p++;
            if (((c1 & 0xC0) == 0x80) && ((c2 & 0xC0) == 0x80))
                cp = ((c0 & 0x0F) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
        } else if ((c0 >> 3) == 0x1E && (p + 2) < e) {
            uint8_t c1 = *p++;
            uint8_t c2 = *p++;
            uint8_t c3 = *p++;
            if (((c1 & 0xC0) == 0x80) && ((c2 & 0xC0) == 0x80) && ((c3 & 0xC0) == 0x80))
                cp = ((c0 & 0x07) << 18) | ((c1 & 0x3F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
        }

        cps.push_back(cp);
    }

    return cps;
}

bool SdfFontAtlasDynamic::createPage() {
    if (m_cfg.pageW == 0 || m_cfg.pageH == 0)
        return false;

    Page p;
    p.w = m_cfg.pageW;
    p.h = m_cfg.pageH;
    p.bitmap.assign((size_t) p.w * (size_t) p.h, 0);

    p.packer.reset(p.w, p.h);

    //const bgfx::Memory* mem = bgfx::copy(p.bitmap.data(), (uint32_t)p.bitmap.size());
    p.tex = bgfx::createTexture2D(
            p.w, p.h,
            false, 1,
            bgfx::TextureFormat::R8,
            BGFX_TEXTURE_NONE,
            nullptr
    );

    if (!bgfx::isValid(p.tex))
        return false;

    m_pages.push_back(std::move(p));
    return true;
}

bool SdfFontAtlasDynamic::rasterizeAndPackGlyph(uint32_t codepoint) {
    if (!m_font)
        return false;

    int gw = 0, gh = 0, xoff = 0, yoff = 0;
    unsigned char* sdf = stbtt_GetCodepointSDF(
            m_font, m_scale,
            (int) codepoint,
            m_cfg.sdfPadding,
            m_cfg.onEdgeValue,
            m_cfg.pixelDistScale,
            &gw, &gh, &xoff, &yoff
    );

    if (!sdf || gw <= 0 || gh <= 0) {
        if (sdf) stbtt_FreeSDF(sdf, nullptr);

        // Create a metrics-only glyph (e.g. space) so pen can advance.
        Glyph g{};
        g.pageIndex = 0;
        g.x = g.y = g.w = g.h = 0;
        g.xoff = 0;
        g.yoff = 0;

        int advW = 0, lsb = 0;
        stbtt_GetCodepointHMetrics(m_font, (int) codepoint, &advW, &lsb);
        g.advanceX = advW * m_scale;

        g.u0 = g.v0 = g.u1 = g.v1 = 0.0f;
        m_glyphs[codepoint] = g;
        return true;
    }

    const uint16_t pad = m_cfg.packingPadding;
    const uint16_t reqW = (uint16_t) (gw + pad * 2);
    const uint16_t reqH = (uint16_t) (gh + pad * 2);

    int packedPage = -1;
    LittleCore::MaxRectsPacker::Rect placed{};

    // Try existing pages
    for (uint16_t i = 0; i < (uint16_t) m_pages.size(); ++i) {
        LittleCore::MaxRectsPacker::Rect r{};
        if (m_pages[i].packer.insert(reqW, reqH, r)) {
            placed = r;
            packedPage = (int) i;
            break;
        }
    }

    // Create new page if needed
    if (packedPage < 0) {
        if (!createPage()) {
            stbtt_FreeSDF(sdf, nullptr);
            return false;
        }

        packedPage = (int) (m_pages.size() - 1);
        LittleCore::MaxRectsPacker::Rect r{};
        if (!m_pages[packedPage].packer.insert(reqW, reqH, r)) {
            stbtt_FreeSDF(sdf, nullptr);
            return false;
        }
        placed = r;
    }

    Page& page = m_pages[(uint16_t) packedPage];

    const uint16_t gx = (uint16_t) (placed.x + pad);
    const uint16_t gy = (uint16_t) (placed.y + pad);

    // Blit into CPU bitmap (optional but useful for debugging / CPU readback)
    for (int row = 0; row < gh; ++row) {
        uint8_t* dst = &page.bitmap[(size_t) (gy + row) * page.w + gx];
        const uint8_t* src = sdf + (row * gw);
        std::copy(src, src + gw, dst);
    }

    /*
    std::vector<uint8_t> output;

    for (int i = 0; i < page.bitmap.size(); ++i) {
        output.push_back(page.bitmap[i]);
        output.push_back(page.bitmap[i]);
        output.push_back(page.bitmap[i]);
        output.push_back(page.bitmap[i]);
    }
    //LittleCore::ImageLoader::SaveTga("FontAtlas.tga", output.data(), page.w, page.h);
     */

    // Upload only the glyph region (gw x gh) at (gx, gy)
    // pitch = gw bytes for R8
    const bgfx::Memory* mem = bgfx::copy(sdf, (uint32_t) (gw * gh));
    bgfx::updateTexture2D(
            page.tex,
            0, 0,
            gx, gy,
            (uint16_t) gw, (uint16_t) gh,
            mem,
            (uint16_t) gw
    );

    stbtt_FreeSDF(sdf, nullptr);

    // Metrics
    Glyph g{};
    g.pageIndex = (uint16_t) packedPage;
    g.x = gx;
    g.y = gy;
    g.w = (uint16_t) gw;
    g.h = (uint16_t) gh;
    g.xoff = (int16_t) xoff;
    g.yoff = (int16_t) yoff;

    int advW = 0, lsb = 0;
    stbtt_GetCodepointHMetrics(m_font, (int) codepoint, &advW, &lsb);
    g.advanceX = advW * m_scale;

    g.u0 = (float) g.x / (float) page.w;
    g.v0 = (float) g.y / (float) page.h;
    g.u1 = (float) (g.x + g.w) / (float) page.w;
    g.v1 = (float) (g.y + g.h) / (float) page.h;

    m_glyphs[codepoint] = g;
    return true;
}
