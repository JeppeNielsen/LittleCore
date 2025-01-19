//
// Created by Jeppe Nielsen on 18/01/2025.
//


#pragma once

#include "ResourceHandle.hpp"
#include "TextureResource.hpp"

namespace LittleCore {
    struct Texturable {
        ResourceHandle<TextureResource> texture;
    };
}