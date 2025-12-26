//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "Prefab.hpp"
#include "Hierarchy.hpp"
#include "RegistrySerializer.hpp"
#include "DefaultResourceManager.hpp"

namespace LittleCore {
    class PrefabSystem : SystemBase {
    public:
        PrefabSystem(entt::registry& registry);
        void Update();
        void Initialize(RegistrySerializerBase& registrySerializer, DefaultResourceManager& resourceManager);
    private:
        RegistrySerializerBase* registrySerializer = nullptr;
        DefaultResourceManager* resourceManager = nullptr;
        void RefreshInstance(entt::entity entity);
        void Clear(Prefab& prefab);
        bool isDestroying = false;
        entt::observer observer;
    };
}
