//
// Created by Jeppe Nielsen on 17/12/2025.
//

#include "RenderableUniforms.hpp"

#include <cstdint>
#include <cstring>   // std::memcmp (not needed here) / std::memcpy
#include <string>

using namespace LittleCore;

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
            // bgfx::TextureHandle is typically a small handle with .idx
            // If yours differs, replace this with the correct stable representation.
            uint16_t idx = e.value.tex.idx;
            h ^= rotl64(mix64(static_cast<uint64_t>(idx)), 17);
            break;
        }
        case K::Vec4:
            h ^= rotl64(hashBytes(&e.value.v4, sizeof(e.value.v4)), 21);
            break;
        case K::Mat3x3:
            h ^= rotl64(hashBytes(&e.value.m3, sizeof(e.value.m3)), 25);
            break;
        case K::Mat4x4:
            h ^= rotl64(hashBytes(&e.value.m4, sizeof(e.value.m4)), 29);
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

void RenderableUniforms::Set(const std::string& id, bgfx::TextureHandle texture) {
    for (auto& e : uniforms) {
        if (e.kind == UniformEntry::Kind::Texture && e.id == id) {
            e.value.tex = texture;
            return;
        }
    }

    UniformEntry entry;
    entry.id = id;
    entry.kind = UniformEntry::Kind::Texture;
    entry.value.tex = texture;
    uniforms.emplace_back(entry);
}

void RenderableUniforms::Set(const std::string& id, vec4 vector) {
    for (auto& e : uniforms) {
        if (e.kind == UniformEntry::Kind::Vec4 && e.id == id) {
            e.value.v4 = vector;
            return;
        }
    }

    UniformEntry entry;
    entry.id = id;
    entry.kind = UniformEntry::Kind::Vec4;
    entry.value.v4 = vector;
    uniforms.emplace_back(entry);
}

void RenderableUniforms::Set(const std::string& id, mat3x3 matrix) {
    for (auto& e : uniforms) {
        if (e.kind == UniformEntry::Kind::Mat3x3 && e.id == id) {
            e.value.m3 = matrix;
            return;
        }
    }

    UniformEntry entry;
    entry.id = id;
    entry.kind = UniformEntry::Kind::Mat3x3;
    entry.value.m3 = matrix;
    uniforms.emplace_back(entry);
}

void RenderableUniforms::Set(const std::string& id, mat4x4 matrix) {
    for (auto& e : uniforms) {
        if (e.kind == UniformEntry::Kind::Mat4x4 && e.id == id) {
            e.value.m4 = matrix;
            return;
        }
    }

    UniformEntry entry;
    entry.id = id;
    entry.kind = UniformEntry::Kind::Mat4x4;
    entry.value.m4 = matrix;
    uniforms.emplace_back(entry);
}

void RenderableUniforms::Remove(const std::string& id) {
    if (auto it = std::find_if(uniforms.begin(), uniforms.end(), [&id](const auto& e) {
            return e.id == id;
    }); it != uniforms.end()) {
        uniforms.erase(it);
    }
}



