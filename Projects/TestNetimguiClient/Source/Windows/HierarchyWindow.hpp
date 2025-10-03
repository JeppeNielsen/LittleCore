//
// Created by Jeppe Nielsen on 29/12/2024.
//


#pragma once
#include "entt/entt.hpp"

class HierarchyWindow {
public:
    void Draw(entt::registry& registry);

private:
    void DrawEntity(entt::registry& registry, entt::entity entity, entt::entity parent);

    struct ReparentedEntity {
        entt::entity entity;
        entt::entity newParent;
    };

    std::vector<ReparentedEntity> reparentedEntities;
    std::vector<entt::entity> entitiesToCreate;
    std::vector<entt::entity> entitiesToDelete;
    entt::entity selectedEntity;

};
