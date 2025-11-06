//
// Created by Jeppe Nielsen on 18/01/2025.
//

#include "UniformCollection.hpp"

using namespace LittleCore;

bgfx::UniformHandle UniformCollection::GetHandle(const std::string& id, bgfx::UniformType::Enum uniformType) {
    auto found = uniforms.find(id);
    if (found!=uniforms.end()) {
        return found->second;
    }
    auto uniform = bgfx::createUniform(id.c_str(), uniformType);
    uniforms.emplace(id, uniform);
    return uniform;
}

void UniformCollection::Clear() {
    for(auto [id, uniform] : uniforms) {
        bgfx::destroy(uniform);
    }
    uniforms.clear();
}

UniformCollection::~UniformCollection() {
    Clear();
}
