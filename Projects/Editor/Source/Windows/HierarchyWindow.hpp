//
// Created by Jeppe Nielsen on 29/12/2024.
//


#pragma once
#include "../ScriptsInclude/RegistryManager.hpp"


class HierarchyWindow {
public:
    HierarchyWindow(RegistryManager& registryManager);

    void DrawGui();

    void Update();

private:
    void DrawEntity(entt::registry& registry, entt::entity entity, entt::entity parent);

    RegistryManager& registryManager;
    RegistryState* currentState;

    struct ReparentedEntity {
        entt::entity entity;
        entt::entity newParent;
    };

    std::vector<ReparentedEntity> reparentedEntities;
    std::vector<entt::entity> entitiesToCreate;
    std::vector<entt::entity> entitiesToDelete;

};
