//
// Created by Jeppe Nielsen on 02/12/2025.
//

#include "FontResource.hpp"
#include <msdfgen-ext.h>
#include <msdfgen.h>
#include "ImageLoader.hpp"

using namespace LittleCore;
using namespace msdfgen;

inline byte pixelFloatToByteHere(float x) {

    //return (byte)255.0f * (x + 0.5f);
    return uint8_t(~int(255.0f-255.f*clamp(x)));
}

std::vector<uint8_t> ConvertToUint8(const BitmapConstSection<float, 3>& bitmap) {
    std::vector<uint8_t> pixels(4*bitmap.width*bitmap.height);
    std::vector<uint8_t>::iterator it = pixels.begin();
    for (int y = 0; y < bitmap.height; ++y) {
        for (int x = 0; x < bitmap.width; ++x) {
            *it++ = pixelFloatToByteHere(bitmap(x, y)[0]);
            *it++ = pixelFloatToByteHere(bitmap(x, y)[1]);
            *it++ = pixelFloatToByteHere(bitmap(x, y)[2]);
            *it++ = 255;
        }
    }
    return pixels;
}

static std::vector<uint8_t> toRGB8(const msdf::BitmapConstSection<float, 3>& bmp) {
    const int w = bmp.width;
    const int h = bmp.height;

    std::vector<uint8_t> out;
    out.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 3u);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const float* p = bmp(x, y);          // p[0]=R, p[1]=G, p[2]=B
            const size_t i = (static_cast<size_t>(y) * w + x) * 3u;

            for (int c = 0; c < 3; ++c) {
                float v = std::clamp(p[c], 0.0f, 1.0f);
                out[i + static_cast<size_t>(c)] =
                        static_cast<uint8_t>(std::lround(v * 255.0f));
            }
        }
    }
    return out;
}


const FontAtlas::GlyphInfo& FontResource::EnsureGlyph(uint32_t glyphId) {

    auto found = fontAtlas.find(glyphId);
    if (found) {
        return *found;
    }

    Shape shape;
    double xAdvance = 0;
    if (!loadGlyph(shape, font, glyphId, FONT_SCALING_EM_NORMALIZED, &xAdvance)) {
        return emptyGlyph;
    }

    shape.normalize();
    edgeColoringSimple(shape, 3.0);


    const double glyphSize = 32.0;
    const double pxRange   = 8.0;      // because Range_em(0.125)*64 = 8
    const double paddingPx = 8.0;     // >= pxRange; 10 is a good safety margin

// 1) Get bounds in EM space
    auto bounds = shape.getBounds(0);  // or shape.getBounds(...) depending on version

    double w = bounds.r - bounds.l;
    double h = bounds.t - bounds.b;

// 2) Compute scale so glyph fits inside (glyphSize - 2*padding)
    double inner = glyphSize - 2.0*paddingPx;
    double scale = inner / std::max(w, h);

// 3) Compute translation so left/bottom go to padding
    Vector2 translateEm(
            paddingPx/scale - bounds.l,
            paddingPx/scale - bounds.b
    );

// 4) Range in EM space must match pxRange
    double rangeEm = pxRange / scale;

    SDFTransformation tr(
            Projection(scale, translateEm),
            Range(rangeEm)
    );

    auto emToPx = [&](double emx, double emy) {
        return Vector2((emx + translateEm.x) * scale,
                       (emy + translateEm.y) * scale);
    };

    Vector2 p0 = emToPx(bounds.l, bounds.b); // left,bottom
    Vector2 p1 = emToPx(bounds.r, bounds.t); // right,top

    double atlasL = std::max(0.0, std::floor(p0.x));
    double atlasB = std::max(0.0, std::floor(p0.y));
    double atlasR = std::min(glyphSize, std::ceil(p1.x));
    double atlasT = std::min(glyphSize, std::ceil(p1.y));

    Bitmap<float, 3> msdf(glyphSize, glyphSize);
    generateMSDF(msdf, shape, tr);

    auto pixels = toRGB8(msdf);

    ImageLoader::SaveTga("GlyphA.tga", pixels.data(), glyphSize, glyphSize);



    FontAtlas::GlyphBounds atlasBoundsPx {
            atlasL, atlasB, atlasR, atlasT
    };

    //double l, b, r, t;
    FontAtlas::GlyphBounds glyphBounds {bounds.l, bounds.b, bounds.r, bounds.t};

    auto glyph = fontAtlas.addGlyph(glyphId, pixels.data(), glyphSize, glyphSize, (float )xAdvance,glyphBounds, atlasBoundsPx);
    return *glyph;
}

FontResource::FontResource(): fontAtlas({}) {

}
