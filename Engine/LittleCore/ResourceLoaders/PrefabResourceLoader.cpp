//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabResourceLoader.hpp"
#include "RegistryHelper.hpp"
#include "FileHelper.hpp"
#include "Hierarchy.hpp"

using namespace LittleCore;

PrefabResourceLoader::PrefabResourceLoader(RegistrySerializerBase& registrySerializer) :
     registrySerializer(registrySerializer) {
}

entt::entity FindRoot(entt::registry& registry) {
    auto view = registry.view<Hierarchy>();
    for(auto[entity, hierachy] : view.each()) {
        if (hierachy.children.size() == 0) {
            return entity;
        }
    }
    return entt::null;
}

void PrefabResourceLoader::Load(PrefabResource& resource) {
    auto json = FileHelper::ReadAllText(path);
    registrySerializer.Deserialize(registry, json);
    resource.registry = &registry;
    resource.root = FindRoot(registry);
}

void PrefabResourceLoader::Unload(PrefabResource& resource) {
    registry.clear();
    resource.root = entt::null;
    resource.registry = nullptr;
}

bool PrefabResourceLoader::IsLoaded() {
    return true;
}

void PrefabResourceLoader::Reload(PrefabResource& resource) {
    Unload(resource);
    Load(resource);
}
