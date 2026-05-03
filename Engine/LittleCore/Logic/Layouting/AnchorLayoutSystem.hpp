//
// Created by Codex on 03/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "SystemBase.hpp"

namespace LittleCore {
    class AnchorLayoutSystem : public SystemBase {
    public:
        AnchorLayoutSystem(entt::registry& registry);
        void Update();

    private:
        void MarkDirty(entt::registry& registry, entt::entity entity);
        void ApplyAnchors(entt::entity entity);

        entt::observer observer;
        bool isDirty;
    };
}
