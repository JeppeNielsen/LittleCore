//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabResourceLoader.hpp"
#include "RegistryHelper.hpp"
#include "FileHelper.hpp"
#include "Hierarchy.hpp"
#include "RegistrySerializer.hpp"
#include "SerializationContext.hpp"
#include "DefaultResourceManager.hpp"
#include <iostream>

using namespace LittleCore;

PrefabResourceLoader::PrefabResourceLoader(RegistrySerializerBase& registrySerializer, void* defaultResourceManager) :
     registrySerializer(registrySerializer),
     hierarchySystem(registry),
     defaultResourceManager(defaultResourceManager){
}

void FindRoots(entt::registry& registry, std::vector<entt::entity>& roots) {
    auto view = registry.view<Hierarchy>();
    for(auto[entity, hierarchy] : view.each()) {
        if (hierarchy.parent == entt::null) {
            roots.push_back(entity);
        }
    }
}

void PrefabResourceLoader::Load(PrefabResource& resource) {
    auto json = FileHelper::ReadAllText(path);
    SerializationContext context {
        .resourceManager = (DefaultResourceManager*)defaultResourceManager
    };
    registrySerializer.Deserialize(registry, json, context);
    hierarchySystem.Update();
    resource.registry = &registry;
    resource.roots.clear();
    FindRoots(registry, resource.roots);
}

void PrefabResourceLoader::Unload(PrefabResource& resource) {
    registry.clear();
    resource.roots.clear();
    resource.registry = nullptr;
}

bool PrefabResourceLoader::IsLoaded() {
    return true;
}

void PrefabResourceLoader::Reload(PrefabResource& resource) {
    Unload(resource);
    Load(resource);
}
