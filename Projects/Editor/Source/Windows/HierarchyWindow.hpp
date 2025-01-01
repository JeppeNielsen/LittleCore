//
// Created by Jeppe Nielsen on 29/12/2024.
//


#pragma once
#include "../ScriptsInclude/RegistryManager.hpp"


class HierarchyWindow {
public:
    HierarchyWindow(RegistryManager& registryManager);

    void DrawGui();

private:
    void DrawEntity(entt::registry& registry, entt::entity entity, entt::entity parent);

    RegistryManager& registryManager;
    RegistryState* currentState;

};
