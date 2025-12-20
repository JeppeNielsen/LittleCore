//
// Created by Jeppe Nielsen on 02/12/2025.
//

#pragma once
#include "FontResource.hpp"
#include "ResourceHandle.hpp"

namespace LittleCore {
    struct Label {
        ResourceHandle<FontResource> font;
        std::string text;
        float outlineSize;
    };
}