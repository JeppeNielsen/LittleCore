//
// Created by Jeppe Nielsen on 18/01/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include "ResourceHandle.hpp"
#include "ResourceComponent.hpp"

namespace LittleCore {
    struct Texturable : public ResourceComponent<Texturable> {
        bgfx::TextureHandle texture = BGFX_INVALID_HANDLE;
    };
}