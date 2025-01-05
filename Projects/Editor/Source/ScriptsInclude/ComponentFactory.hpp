//
// Created by Jeppe Nielsen on 04/01/2025.
//


#pragma once
#include <functional>
#include <entt/entt.hpp>
#include <string>
#include <vector>

class ComponentFactory {
public:
    using CreateComponentFunction = std::function<void(entt::registry&, entt::entity, const std::string&)>;
    using GetComponentIdsFunction = std::function<std::vector<std::string>()>;

    void Install(CreateComponentFunction createFunction, GetComponentIdsFunction getIdsFunction);
    void CreateComponent(entt::registry& registry, entt::entity entity, const std::string& componentId);
    std::vector<std::string> GetComponentIds();

    void Clear();

private:
    std::vector<CreateComponentFunction> createFunctions;
    std::vector<GetComponentIdsFunction> getIdsFunctions;
};
