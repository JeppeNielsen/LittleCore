//
// Created by Codex on 03/05/2026.
//

#pragma once

#include <entt/entt.hpp>
#include "SystemBase.hpp"

namespace LittleCore {
    class LayoutSystem : public SystemBase {
    public:
        LayoutSystem(entt::registry& registry);

        void Update();

    private:
        void InitializeLayouter(entt::entity entity);
        void MarkDirty(entt::registry& registry, entt::entity entity);
        void MarkDirtyOnDestroy(entt::registry& registry, entt::entity entity);
        void ApplyRectLayout(entt::entity entity);

        entt::observer observer;
        bool isDirty;
    };
}
