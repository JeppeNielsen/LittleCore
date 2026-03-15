//
// Created by Jeppe Nielsen on 17/12/2025.
//


#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "SokolDirect.hpp"
#include "Math.hpp"
#include "Color.hpp"

namespace LittleCore {

    class RenderableUniforms {
    public:

        struct UniformEntry {
            enum class Kind { Texture, Value };

            std::string id;
            Kind kind = Kind::Value;
            sg_uniform_type type = SG_UNIFORMTYPE_INVALID;
            uint16_t arrayCount = 1;
            sg_image texture = {SG_INVALID_ID};
            std::vector<uint8_t> data;
        };

        using UniformList = std::vector<UniformEntry>;

        const UniformList& GetUniforms() const;

        void SetRaw(const std::string& id, sg_uniform_type type, const void* data, uint16_t arrayCount = 1);
        void Set(const std::string& id, sg_image texture);
        void Set(const std::string& id, float value);
        void Set(const std::string& id, vec2 vector);
        void Set(const std::string& id, vec3 vector);
        void Set(const std::string& id, vec4 vector);
        void Set(const std::string& id, int value);
        void Set(const std::string& id, ivec2 vector);
        void Set(const std::string& id, ivec3 vector);
        void Set(const std::string& id, ivec4 vector);
        void Set(const std::string& id, mat4x4 matrix);
        void Remove(const std::string& id);

        uint64_t CalculateHash() const;

    private:
        UniformEntry* FindEntry(const std::string& id);
        const UniformEntry* FindEntry(const std::string& id) const;

        UniformList uniforms;
    };

}
