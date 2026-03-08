//
// Created by Jeppe Nielsen on 17/12/2025.
//


#pragma once
#include <string>
#include <unordered_map>
#include <tuple>
#include "SokolDirect.hpp"
#include "Math.hpp"
#include <vector>
#include "Color.hpp"

namespace LittleCore {

    class RenderableUniforms {
    public:

        struct UniformEntry {
            std::string id;
            enum class Kind { Texture, Vec4, Mat3x3, Mat4x4 } kind;

            union Value {
                sg_image tex;
                vec4 v4;
                mat3x3 m3;
                mat4x4 m4;

                Value() {}
            } value;
        };

        using UniformList = std::vector<UniformEntry>;

        const UniformList& GetUniforms() const;

        void Set(const std::string& id, sg_image texture);
        void Set(const std::string& id, vec4 vector);
        void Set(const std::string& id, mat3x3 matrix);
        void Set(const std::string& id, mat4x4 matrix);
        void Remove(const std::string& id);

        uint64_t CalculateHash() const;

    private:

        UniformList uniforms;
    };

}
