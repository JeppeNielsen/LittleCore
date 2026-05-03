//
// Created by Codex on 03/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "SystemBase.hpp"

namespace LittleCore {
    struct SizableSystem : SystemBase {
        SizableSystem(entt::registry& registry);

        void Update();

    private:
        void SizableAdded(entt::registry& registry, entt::entity entity);
        void MeshAdded(entt::registry& registry, entt::entity entity);
        entt::observer observer;
    };
}
