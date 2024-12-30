//
// Created by Jeppe Nielsen on 30/12/2024.
//

#include "RegistryManager.hpp"

void RegistryManager::Add(const std::string& id, const std::shared_ptr<entt::registry>& registry) {
    auto& state = states[id];
    state.registry = std::weak_ptr<entt::registry>(registry);
}

RegistryState& RegistryManager::Get(const std::string& id) {
    return states[id];
}

void RegistryManager::Clear() {
    states.clear();
}

bool RegistryManager::HasState(const std::string& id) {
    return states.find(id) != states.end();
}

bool RegistryManager::TryGetFirstId(std::string& id) {
    for(auto it : states) {
        id = it.first;
        return true;
    }
    return false;
}
