//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "Prefab.hpp"
#include "Hierarchy.hpp"
#include "RegistrySerializer.hpp"

namespace LittleCore {
    class PrefabSystem : SystemBase {
    public:
        PrefabSystem(entt::registry& registry);
        void Update();
        void SetSerializer(RegistrySerializerBase& registrySerializer);
    private:
        RegistrySerializerBase* registrySerializer = nullptr;
        void RefreshInstance(entt::entity entity);
        entt::observer observer;
    };
}
