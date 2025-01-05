//
// Created by Jeppe Nielsen on 04/01/2025.
//

#include "ComponentFactory.hpp"

void ComponentFactory::Install(ComponentFactory::CreateComponentFunction createFunction,
                               ComponentFactory::GetComponentIdsFunction getIdsFunction) {
    createFunctions.push_back(createFunction);
    getIdsFunctions.push_back(getIdsFunction);
}

void ComponentFactory::CreateComponent(entt::registry& registry, entt::entity entity, const std::string& componentId) {
    for (auto& function: createFunctions) {
        function(registry, entity, componentId);
    }
}

std::vector<std::string> ComponentFactory::GetComponentIds() {
    std::vector<std::string> ids;
    for(auto& function : getIdsFunctions) {
        auto otherIds = function();
        ids.insert(ids.begin(), otherIds.begin(), otherIds.end());
    }
    return ids;
}

void ComponentFactory::Clear() {
    createFunctions.clear();
    getIdsFunctions.clear();
}
