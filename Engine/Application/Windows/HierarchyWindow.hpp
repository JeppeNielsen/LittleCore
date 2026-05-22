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
        struct EntityMove {
            entt::entity entity;
            entt::entity newParent;
            entt::entity anchorSibling = entt::null;
            bool insertAfterAnchor = false;
        };

        void DrawEntity(EditorSimulation& simulation, entt::entity entity, entt::entity parent);
        void QueueEntityMove(entt::registry& registry, entt::entity entity, entt::entity newParent,
                             entt::entity anchorSibling = entt::null, bool insertAfterAnchor = false);
        bool ApplyEntityMove(entt::registry& registry, const EntityMove& move);

        std::vector<EntityMove> entityMoves;
        std::vector<entt::entity> entitiesToCreate;
        std::vector<entt::entity> entitiesToDelete;
        std::vector<entt::entity> entitiesToDuplicate;


    };
}
