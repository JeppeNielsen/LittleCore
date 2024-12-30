//
// Created by Jeppe Nielsen on 30/12/2024.
//


#pragma once
#include <entt/entt.hpp>
#include <string>
#include <memory>
#include <unordered_map>

struct RegistryState {
    std::weak_ptr<entt::registry> registry;
    entt::entity selectedEntity = entt::null;
};

class RegistryManager {
public:
    void Add(const std::string& id, const std::shared_ptr<entt::registry>& registry);
    RegistryState& Get(const std::string& id);
    void Clear();
    bool HasState(const std::string& id);
    bool TryGetFirstId(std::string& id);
private:
    using States = std::unordered_map<std::string, RegistryState>;
    States states;
};
