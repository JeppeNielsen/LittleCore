//
// Created by Jeppe Nielsen on 29/12/2024.
//


#pragma once
#include <vector>
#include "entt/entt.hpp"

namespace LittleCore {

    class EditorSimulation;

    class HierarchyWindow {
    public:
        void Draw(EditorSimulation& simulation);

    private:
        void DrawEntity(EditorSimulation& simulation, entt::entity entity, entt::entity parent);

        struct ReparentedEntity {
            entt::entity entity;
            entt::entity newParent;
        };

        std::vector<ReparentedEntity> reparentedEntities;
        std::vector<entt::entity> entitiesToCreate;
        std::vector<entt::entity> entitiesToDelete;
        std::vector<entt::entity> entitiesToDuplicate;


    };
}