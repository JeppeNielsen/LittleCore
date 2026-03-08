//
// Created by Jeppe Nielsen on 18/01/2025.
//


#pragma once
#include <unordered_map>
#include <string>
#include <cstdint>

namespace LittleCore {
    class UniformCollection {
    public:
        ~UniformCollection();

        using UniformHandle = uint32_t;
        using Uniforms = std::unordered_map<std::string, UniformHandle>;

        UniformHandle GetHandle(const std::string& id);

        void Clear();

    private:
        Uniforms uniforms;
        UniformHandle nextHandle = 1;
    };
}
