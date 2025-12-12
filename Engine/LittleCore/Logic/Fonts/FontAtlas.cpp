//
// Created by Jeppe Nielsen on 02/12/2025.
//

#include "FontAtlas.hpp"
#include "ImageLoader.hpp"

using namespace LittleCore;

FontAtlas::FontAtlas(const Params& params)
        : params(params) {
    reset(params.initialWidth, params.initialHeight);
}

FontAtlas::~FontAtlas() { destroyTexture(); }

void FontAtlas::reset(uint16_t newWidth, uint16_t newHeight) {
    destroyTexture();

    atlasWidth = newWidth;
    atlasHeight = newHeight;
    bytesPerPixel = bytesPerPixelFor(params.format);

    glyphs.clear();
    packer.reset(atlasWidth, atlasHeight);

    if (params.keepCpuCopy) {
        cpuPixels.assign(size_t(atlasWidth)*atlasHeight*bytesPerPixel, 0);
    } else {
        cpuPixels.clear();
    }

    textureHandle = bgfx::createTexture2D(
            atlasWidth, atlasHeight,
            false, 1,
            params.format,
            params.flags,
            nullptr
    );
    bgfx::setName(textureHandle, params.debugName);
}

bgfx::TextureHandle FontAtlas::texture() const { return textureHandle; }
uint16_t FontAtlas::width() const { return atlasWidth; }
uint16_t FontAtlas::height() const { return atlasHeight; }

const FontAtlas::GlyphInfo* FontAtlas::find(uint32_t glyphId) const {
    auto it = glyphs.find(glyphId);
    return it == glyphs.end() ? nullptr : &it->second;
}

const FontAtlas::GlyphInfo* FontAtlas::addGlyph(
        uint32_t glyphId,
        const uint8_t* pixels,
        uint16_t glyphWidth,
        uint16_t glyphHeight,
        float advance,
        const GlyphBounds& bounds,
        const GlyphBounds& atlasBounds
) {
    auto it = glyphs.find(glyphId);
    if (it != glyphs.end())
        return &it->second;

    if (glyphWidth == 0 || glyphHeight == 0) {
        GlyphInfo empty;
        empty.advance = advance;
        empty.valid = true;
        auto [eIt, _] = glyphs.emplace(glyphId, empty);
        return &eIt->second;
    }

    uint16_t packedWidth  = glyphWidth  + params.padding*2;
    uint16_t packedHeight = glyphHeight + params.padding*2;

    MaxRectsPacker::Rect packedRect;
    if (!packer.insert(packedWidth, packedHeight, packedRect)) {
        if (!growToFit(packedWidth, packedHeight)) return nullptr;
        if (!packer.insert(packedWidth, packedHeight, packedRect)) return nullptr;
    }

    uint16_t destX = packedRect.x + params.padding;
    uint16_t destY = packedRect.y + params.padding;

    blitToCpu(destX, destY, glyphWidth, glyphHeight, pixels);
    uploadRegion(destX, destY, glyphWidth, glyphHeight, pixels);

    ImageLoader::SaveTga("CpuPixels.tga", cpuPixels.data(), atlasWidth, atlasHeight);

    GlyphInfo info;
    info.valid = true;
    info.x = destX;
    info.y = destY;
    info.width  = glyphWidth;
    info.height = glyphHeight;
    info.advance = advance;
    info.bounds = bounds;
    info.atlasBounds = atlasBounds;
    computeUVs(info);
    auto [newIt, _] = glyphs.emplace(glyphId, info);
    return &newIt->second;
}

uint8_t FontAtlas::bytesPerPixelFor(bgfx::TextureFormat::Enum format) {
    using F = bgfx::TextureFormat;
    switch (format) {
        case F::R8: return 1;
        case F::RG8: return 2;
        case F::RGBA8: return 4;
        case F::BGRA8: return 4;
        case F::R16F: return 2;
        case F::RG16F: return 4;
        case F::RGBA16F: return 8;
        default: return 4;
    }
}

void FontAtlas::destroyTexture() {
    if (bgfx::isValid(textureHandle)) {
        bgfx::destroy(textureHandle);
        textureHandle = BGFX_INVALID_HANDLE;
    }
}

void FontAtlas::computeUVs(GlyphInfo& info) const {
    float invW = 1.0f / float(atlasWidth);
    float invH = 1.0f / float(atlasHeight);
    float halfX = 0.5f / (float)atlasWidth;
    float halfY = 0.5f / (float)atlasHeight;


    info.u0 = (info.x + info.atlasBounds.l + halfX) * invW;
    info.v0 = (info.y + info.atlasBounds.b + halfY) * invH;
    info.u1 = (info.x + info.atlasBounds.r + halfX) * invW;
    info.v1 = (info.y + info.atlasBounds.t + halfY) * invH;
}

bool FontAtlas::growToFit(uint16_t neededWidth, uint16_t neededHeight) {
    if (!params.keepCpuCopy) return false;

    uint16_t newWidth = atlasWidth;
    uint16_t newHeight = atlasHeight;

    while (neededWidth > newWidth || neededHeight > newHeight) {
        if (newWidth <= newHeight) newWidth *= 2;
        else newHeight *= 2;

        if (newWidth > params.maxSize || newHeight > params.maxSize)
            return false;
    }

    rebuildTexture(newWidth, newHeight);
    return true;
}

void FontAtlas::rebuildTexture(uint16_t newWidth, uint16_t newHeight) {
    std::vector<uint8_t> newCpu(size_t(newWidth)*newHeight*bytesPerPixel, 0);

    for (uint16_t row = 0; row < atlasHeight; ++row) {
        const uint8_t* src = &cpuPixels[size_t(row)*atlasWidth*bytesPerPixel];
        uint8_t* dst = &newCpu[size_t(row)*newWidth*bytesPerPixel];
        std::memcpy(dst, src, size_t(atlasWidth)*bytesPerPixel);
    }

    destroyTexture();

    atlasWidth = newWidth;
    atlasHeight = newHeight;
    cpuPixels.swap(newCpu);

    // Note: We do NOT repack existing glyphs, positions remain valid.
    // We just expand the bin for future inserts.
    auto oldUsed = packer.usedRects();
    packer.reset(atlasWidth, atlasHeight);
    for (const auto& r : oldUsed) {
        // Lightweight re-marking so packer avoids old areas.
        MaxRectsPacker::Rect dummy;
        packer.insert(r.width, r.height, dummy);
    }

    const bgfx::Memory* mem = bgfx::copy(cpuPixels.data(), uint32_t(cpuPixels.size()));

    textureHandle = bgfx::createTexture2D(
            atlasWidth, atlasHeight,
            true, 1,
            params.format,
            params.flags,
            mem
    );
    bgfx::setName(textureHandle, params.debugName);

    for (auto& kv : glyphs)
        computeUVs(kv.second);
}

void FontAtlas::blitToCpu(uint16_t destX, uint16_t destY,
               uint16_t glyphWidth, uint16_t glyphHeight,
               const uint8_t* pixels) {
    if (!params.keepCpuCopy) return;

    for (uint16_t row = 0; row < glyphHeight; ++row) {
        uint8_t* dst =
                &cpuPixels[(size_t(destY + row)*atlasWidth + destX)*bytesPerPixel];
        const uint8_t* src =
                &pixels[size_t(row)*glyphWidth*bytesPerPixel];
        std::memcpy(dst, src, size_t(glyphWidth)*bytesPerPixel);
    }
}

void FontAtlas::uploadRegion(uint16_t destX, uint16_t destY,
                  uint16_t glyphWidth, uint16_t glyphHeight,
                  const uint8_t* pixels) {
    const bgfx::Memory* mem =
            bgfx::copy(pixels, uint32_t(glyphWidth)*glyphHeight*bytesPerPixel);

    bgfx::updateTexture2D(
            textureHandle,
            0, 0,
            destX, destY,
            glyphWidth, glyphHeight,
            mem
    );
}

