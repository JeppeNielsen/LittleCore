//
// Created by Jeppe Nielsen on 20/12/2025.
//


#pragma once
#include "entt/entt.hpp"
#include "SystemBase.hpp"
#include "Colorable.hpp"
#include "Renderable.hpp"

namespace LittleCore {
    struct ColorableSystem : SystemBase {
        ColorableSystem(entt::registry& registry);
        void Update();
    private:
        void EntityDestroyed(entt::registry& reg, entt::entity entity);
        entt::observer observer;

    };
}
