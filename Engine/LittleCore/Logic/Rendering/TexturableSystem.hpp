//
// Created by Jeppe Nielsen on 19/12/2025.
//


#pragma once
#include "SystemBase.hpp"
#include "Texturable.hpp"
#include "Renderable.hpp"

namespace LittleCore {
    struct TexturableSystem : SystemBase {
        TexturableSystem(entt::registry& registry);
        void Update();
        void Reload();
    private:
        void EntityDestroyed(entt::registry& reg, entt::entity entity);
        entt::observer observer;
    };
}
