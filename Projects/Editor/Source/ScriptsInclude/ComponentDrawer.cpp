//
// Created by Jeppe Nielsen on 01/01/2025.
//

#include "ComponentDrawer.hpp"

void ComponentDrawer::Install(ComponentDrawer::ComponentDrawerFunction function) {
    functions.push_back(function);
}

void ComponentDrawer::Draw(entt::registry& registry, entt::entity entity) {
    for(auto& function : functions) {
        function(registry, entity);
    }
}

void ComponentDrawer::Clear() {
    functions.clear();
}
