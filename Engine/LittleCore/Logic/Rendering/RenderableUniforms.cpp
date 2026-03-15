//
// Created by Jeppe Nielsen on 17/12/2025.
//

#include "RenderableUniforms.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>

using namespace LittleCore;

namespace {
    std::size_t GetPackedUniformElementSize(const sg_uniform_type type) noexcept {
        switch (type) {
            case SG_UNIFORMTYPE_FLOAT:
                return sizeof(float);
            case SG_UNIFORMTYPE_FLOAT2:
                return sizeof(float) * 2;
            case SG_UNIFORMTYPE_FLOAT3:
                return sizeof(float) * 3;
            case SG_UNIFORMTYPE_FLOAT4:
                return sizeof(float) * 4;
            case SG_UNIFORMTYPE_INT:
                return sizeof(int);
            case SG_UNIFORMTYPE_INT2:
                return sizeof(int) * 2;
            case SG_UNIFORMTYPE_INT3:
                return sizeof(int) * 3;
            case SG_UNIFORMTYPE_INT4:
                return sizeof(int) * 4;
            case SG_UNIFORMTYPE_MAT4:
                return sizeof(mat4x4);
            default:
                return 0;
        }
    }
}

static inline uint64_t rotl64(uint64_t x, int r) noexcept {
    return (x << r) | (x >> (64 - r));
}

static inline uint64_t mix64(uint64_t x) noexcept {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccdULL;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53ULL;
    x ^= x >> 33;
    return x;
}

static inline uint64_t hashBytes(const void* data, size_t n) noexcept {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    uint64_t h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        h ^= p[i];
        h *= 1099511628211ULL;
    }
    return mix64(h);
}

static inline uint64_t hashString(const std::string& s) noexcept {
    return hashBytes(s.data(), s.size());
}

// Hash one UniformEntry (depends on kind+id+value)
static inline uint64_t hashEntry(const RenderableUniforms::UniformEntry& e) noexcept {
    using K = RenderableUniforms::UniformEntry::Kind;

    uint64_t h = 0;
    h ^= mix64(static_cast<uint64_t>(static_cast<int>(e.kind)) + 0x9e3779b97f4a7c15ULL);
    h ^= rotl64(hashString(e.id), 1);

    switch (e.kind) {
        case K::Texture: {
            h ^= rotl64(mix64(static_cast<uint64_t>(e.texture.id)), 17);
            break;
        }
        case K::Value:
            h ^= rotl64(mix64(static_cast<uint64_t>(static_cast<int>(e.type))), 21);
            h ^= rotl64(mix64(static_cast<uint64_t>(e.arrayCount)), 25);
            if (!e.data.empty()) {
                h ^= rotl64(hashBytes(e.data.data(), e.data.size()), 29);
            }
            break;
    }

    return mix64(h);
}

uint64_t RenderableUniforms::CalculateHash() const {

    uint64_t fp = mix64(static_cast<uint64_t>(uniforms.size()));

    for (const auto& e : uniforms) {
        fp ^= hashEntry(e);
    }

    return mix64(fp);
}

const RenderableUniforms::UniformList& RenderableUniforms::GetUniforms() const {
    return uniforms;
}

RenderableUniforms::UniformEntry* RenderableUniforms::FindEntry(const std::string& id) {
    const auto it = std::find_if(uniforms.begin(), uniforms.end(), [&id](const auto& entry) {
        return entry.id == id;
    });
    return it != uniforms.end() ? &(*it) : nullptr;
}

const RenderableUniforms::UniformEntry* RenderableUniforms::FindEntry(const std::string& id) const {
    const auto it = std::find_if(uniforms.begin(), uniforms.end(), [&id](const auto& entry) {
        return entry.id == id;
    });
    return it != uniforms.end() ? &(*it) : nullptr;
}

void RenderableUniforms::SetRaw(const std::string& id, const sg_uniform_type type, const void* data, const uint16_t arrayCount) {
    if (type == SG_UNIFORMTYPE_INVALID || data == nullptr || arrayCount == 0) {
        return;
    }

    const std::size_t elementSize = GetPackedUniformElementSize(type);
    if (elementSize == 0) {
        return;
    }

    UniformEntry* entry = FindEntry(id);
    if (entry == nullptr) {
        uniforms.emplace_back();
        entry = &uniforms.back();
        entry->id = id;
    }

    entry->kind = UniformEntry::Kind::Value;
    entry->type = type;
    entry->arrayCount = arrayCount;
    entry->texture = {SG_INVALID_ID};
    entry->data.resize(elementSize * arrayCount);
    std::memcpy(entry->data.data(), data, entry->data.size());
}

void RenderableUniforms::Set(const std::string& id, sg_image texture) {
    UniformEntry* entry = FindEntry(id);
    if (entry == nullptr) {
        uniforms.emplace_back();
        entry = &uniforms.back();
        entry->id = id;
    }

    entry->kind = UniformEntry::Kind::Texture;
    entry->type = SG_UNIFORMTYPE_INVALID;
    entry->arrayCount = 1;
    entry->texture = texture;
    entry->data.clear();
}

void RenderableUniforms::Set(const std::string& id, float value) {
    SetRaw(id, SG_UNIFORMTYPE_FLOAT, &value);
}

void RenderableUniforms::Set(const std::string& id, vec2 vector) {
    SetRaw(id, SG_UNIFORMTYPE_FLOAT2, &vector);
}

void RenderableUniforms::Set(const std::string& id, vec3 vector) {
    SetRaw(id, SG_UNIFORMTYPE_FLOAT3, &vector);
}

void RenderableUniforms::Set(const std::string& id, vec4 vector) {
    SetRaw(id, SG_UNIFORMTYPE_FLOAT4, &vector);
}

void RenderableUniforms::Set(const std::string& id, int value) {
    SetRaw(id, SG_UNIFORMTYPE_INT, &value);
}

void RenderableUniforms::Set(const std::string& id, ivec2 vector) {
    SetRaw(id, SG_UNIFORMTYPE_INT2, &vector);
}

void RenderableUniforms::Set(const std::string& id, ivec3 vector) {
    SetRaw(id, SG_UNIFORMTYPE_INT3, &vector);
}

void RenderableUniforms::Set(const std::string& id, ivec4 vector) {
    SetRaw(id, SG_UNIFORMTYPE_INT4, &vector);
}

void RenderableUniforms::Set(const std::string& id, mat4x4 matrix) {
    SetRaw(id, SG_UNIFORMTYPE_MAT4, &matrix);
}

void RenderableUniforms::Remove(const std::string& id) {
    if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [&id](const auto& e) {
            return e.id == id;
    }); it != uniforms.end()) {
        uniforms.erase(it);
    }
}
