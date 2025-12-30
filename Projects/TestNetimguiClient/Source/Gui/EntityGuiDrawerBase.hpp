//
// Created by Jeppe Nielsen on 29/12/2025.
//

#pragma once
#include "EntityGuiDrawerContext.hpp"

namespace LittleCore {
    struct EntityGuiDrawerBase {

        EntityGuiDrawerBase() = default;

        virtual ~EntityGuiDrawerBase() = default;

        virtual bool Draw(entt::registry& registry, entt::entity entity) = 0;

        virtual bool DrawComponentMenu(entt::registry& registry, entt::entity entity) = 0;

        void Initialize(EntityGuiDrawerContext* context) {
            this->context = context;
        }

    protected:
        EntityGuiDrawerContext* context;
    };
}