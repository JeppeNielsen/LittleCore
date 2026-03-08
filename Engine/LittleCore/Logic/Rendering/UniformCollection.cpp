//
// Created by Jeppe Nielsen on 18/01/2025.
//

#include "UniformCollection.hpp"

using namespace LittleCore;

UniformCollection::UniformHandle UniformCollection::GetHandle(const std::string& id) {
    auto found = uniforms.find(id);
    if (found!=uniforms.end()) {
        return found->second;
    }
    auto uniform = nextHandle++;
    uniforms.emplace(id, uniform);
    return uniform;
}

void UniformCollection::Clear() {
    uniforms.clear();
}

UniformCollection::~UniformCollection() {
    Clear();
}
