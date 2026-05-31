//
// Created by Jeppe Nielsen on 29/05/2026.
//


#pragma once
#include "SystemBase.hpp"

namespace LittleCore {
    class ClickableBoundingBoxSystem : SystemBase {
    public:
        ClickableBoundingBoxSystem(entt::registry &registry);
        void Update();
    private:
        entt::observer observer;
    };
}
