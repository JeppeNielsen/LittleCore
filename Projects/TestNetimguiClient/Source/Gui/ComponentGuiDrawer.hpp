//
// Created by Jeppe Nielsen on 25/09/2025.
//

#pragma once
#include "ObjectGuiDrawer.hpp"
//#include <entt/entity/entity.hpp>
//#include <entt/entity/registry.hpp>
#include "BoundingBox.hpp"

namespace LittleCore {

    struct IComponentGuiDrawer {

        virtual ~IComponentGuiDrawer() {

        }

        virtual void Draw(entt::registry& registry, entt::entity entity) = 0;

    };

    template<typename T>
    struct ComponentGuiDrawer : IComponentGuiDrawer{

        std::string name;

        ~ComponentGuiDrawer() override { }

        void Draw(entt::registry& registry, entt::entity entity) override {
            GuiHelper::DrawHeader(name);
            T& component = registry.get<T>(entity);
            ObjectGuiDrawer::Draw(component);
        }

    };



}
