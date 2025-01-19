//
// Created by Jeppe Nielsen on 18/01/2025.
//


#pragma once
#include <unordered_map>
#include <string>
#include <bgfx/bgfx.h>

namespace LittleCore {
    class UniformCollection {
    public:
        using Uniforms = std::unordered_map<std::string, bgfx::UniformHandle>;

        bgfx::UniformHandle GetHandle(const std::string& id, bgfx::UniformType::Enum uniformType);

    private:
        Uniforms uniforms;
    };
}
