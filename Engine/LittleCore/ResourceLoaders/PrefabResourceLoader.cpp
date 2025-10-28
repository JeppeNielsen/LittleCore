//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabResourceLoader.hpp"
#include "RegistryHelper.hpp"
#include "FileHelper.hpp"
#include "Hierarchy.hpp"

using namespace LittleCore;

PrefabResourceLoader::PrefabResourceLoader(RegistrySerializerBase& registrySerializer) :
     registrySerializer(registrySerializer),
     hierarchySystem(registry){
}

void FindRoots(entt::registry& registry, std::vector<entt::entity>& roots) {
    auto view = registry.view<Hierarchy>();
    for(auto[entity, hierarchy] : view.each()) {
        if (hierarchy.children.size() == 0) {
            roots.push_back(entity);
        }
    }
}

void PrefabResourceLoader::Load(PrefabResource& resource) {
    auto json = FileHelper::ReadAllText(path);
    registrySerializer.Deserialize(registry, json);
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
