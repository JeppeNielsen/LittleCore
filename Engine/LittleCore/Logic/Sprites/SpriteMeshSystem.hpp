//
// Created by Codex on 03/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "SystemBase.hpp"

namespace LittleCore {
    struct SpriteMeshSystem : SystemBase {
        SpriteMeshSystem(entt::registry& registry);

        void Update();

    private:
        void SizableAdded(entt::registry& registry, entt::entity entity);
        void MeshAdded(entt::registry& registry, entt::entity entity);
        void SpriteAdded(entt::registry& registry, entt::entity entity);
        entt::observer observer;
    };
}
