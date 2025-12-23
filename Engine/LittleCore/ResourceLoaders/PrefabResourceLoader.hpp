//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceLoader.hpp"
#include "HierarchySystem.hpp"
#include "PrefabResource.hpp"

namespace LittleCore {
    struct RegistrySerializerBase;

    class PrefabResourceLoader : public IResourceLoader<PrefabResource> {
    public:
        PrefabResourceLoader(RegistrySerializerBase& registrySerializer);
    public:
        void Load(PrefabResource& resource) override;
        void Unload(PrefabResource& resource) override;
        bool IsLoaded() override;
        void Reload(PrefabResource& resource) override;

        entt::registry registry;
        HierarchySystem hierarchySystem;
        RegistrySerializerBase& registrySerializer;
    };
}
