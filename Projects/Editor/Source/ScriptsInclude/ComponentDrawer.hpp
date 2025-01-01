//
// Created by Jeppe Nielsen on 01/01/2025.
//


#pragma once
#include <functional>
#include <entt/entt.hpp>
#include <vector>

class ComponentDrawer {
public:
    using ComponentDrawerFunction = std::function<void(entt::registry&, entt::entity)>;

    void Install(ComponentDrawerFunction function);
    void Draw(entt::registry& registry, entt::entity entity);

    void Clear();

private:
    std::vector<ComponentDrawerFunction> functions;
};
