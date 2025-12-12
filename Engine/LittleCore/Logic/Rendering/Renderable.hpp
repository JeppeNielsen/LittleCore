//
// Created by Jeppe Nielsen on 21/01/2024.
//
#pragma once
#include "ResourceHandle.hpp"
#include "ShaderResource.hpp"
#include "BlendMode.hpp"

namespace LittleCore {

    struct Renderable {

        ResourceHandle<ShaderResource> shader;
        BlendMode blendMode = BlendMode::Off;

    };

}