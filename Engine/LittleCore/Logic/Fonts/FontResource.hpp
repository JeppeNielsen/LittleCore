//
// Created by Jeppe Nielsen on 29/11/2025.
//

#pragma once
#include <msdfgen-ext.h>
#include "FontAtlas.hpp"
#include "SDFFontAtlasDynamic.hpp"

namespace LittleCore {
    struct FontResource {

        FontResource();

        msdfgen::FontHandle* font = nullptr;
        FontAtlas fontAtlas;
        int glyphSize = 64;

        const FontAtlas::GlyphInfo& EnsureGlyph(uint32_t glyphId);
        FontAtlas::GlyphInfo emptyGlyph;

        SdfFontAtlasDynamic atlasDynamic;
    };
}