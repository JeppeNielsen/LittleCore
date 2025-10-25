//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "PrefabInstance.hpp"
#include "Hierarchy.hpp"

namespace LittleCore {
    class PrefabSystem : SystemBase {
    public:
        PrefabSystem(entt::registry& registry);
        void Update();
    private:
        void RefreshInstance(entt::entity entity);
        entt::observer observer;
    };
}
