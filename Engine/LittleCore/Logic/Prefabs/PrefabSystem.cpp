//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabSystem.hpp"
#include "RegistryHelper.hpp"
#include "IgnoreSerialization.hpp"
#include "PrefabExposedComponents.hpp"

using namespace LittleCore;

PrefabSystem::PrefabSystem(entt::registry& registry) : SystemBase(registry),
    observer(registry, entt::collector
            .update<Prefab>().where<Hierarchy>()
            .group<Prefab, Hierarchy>()) {
    registry.on_destroy<Prefab>().connect<&PrefabSystem::EntityDestroyed>(this);
}

void PrefabSystem::Update() {
    if (observer.empty()) {
        return;
    }

    while (!observer.empty()) {
        std::vector<entt::entity> changed(observer.begin(), observer.end());
        observer.clear();
        for (auto e : changed) {
            RefreshInstance(e);
        }
    }
}

void PrefabSystem::Clear(Prefab& prefab) {
    isDestroying = true;
    for(auto root : prefab.roots) {
        registry.destroy(root);
    }
    prefab.roots.clear();
    isDestroying = false;
}

void PrefabSystem::RefreshInstance(entt::entity entity) {
    Prefab& prefab = registry.get<Prefab>(entity);
    Clear(prefab);
    if (!prefab.resource) {
        return;
    }

    auto& resource = *prefab.resource.operator->();

    for(auto rootToDuplicate : resource.roots) {
        auto root = RegistryHelper::Duplicate(*resource.registry, rootToDuplicate, registry, [&] (auto source, auto dest){
            if (!resource.registry->any_of<PrefabExposedComponents>(source)) {
                return;
            }

            PrefabExposedComponents& prefabExposedComponents = resource.registry->get<PrefabExposedComponents>(source);



            for(auto& exposedComponent : prefabExposedComponents.exposedComponents) {

                auto it = std::find_if(prefab.components.begin(),
                                       prefab.components.end(), [&](SerializedPrefabComponent& element) {
                            return element.sourceEntity == source && element.componentId == exposedComponent;
                        });

                if (it == prefab.components.end()) {
                   /* prefab.components.push_back({
                            source,
                            dest,
                            exposedComponent,
                            "{}"
                    });
                    */
                }else {
                    it->entity = dest;
                }
            }
        });

        RegistryHelper::TraverseHierarchy(registry, root, [&](auto child) {
            registry.emplace<IgnoreSerialization>(child);
        });

        prefab.roots.push_back(root);

        auto& rootHierarchy = registry.get<Hierarchy>(root);
        rootHierarchy.parent = entity;

        if (registrySerializer != nullptr) {
            for (SerializedPrefabComponent& serializedPrefabComponent: prefab.components) {
                registrySerializer->DeserializeComponent(registry, serializedPrefabComponent.entity, serializedPrefabComponent.componentId, serializedPrefabComponent.data);
            }
        }
    }
}

void PrefabSystem::SetSerializer(RegistrySerializerBase& registrySerializer) {
    this->registrySerializer = &registrySerializer;
}

void PrefabSystem::EntityDestroyed(entt::registry& registry, entt::entity entity) {
    if (isDestroying) {
        return;
    }
    Prefab& prefab = registry.get<Prefab>(entity);
    Clear(prefab);
}
