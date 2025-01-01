//
// Created by Jeppe Nielsen on 01/01/2025.
//


#pragma once
#include <functional>
#include "../ScriptsInclude/RegistryManager.hpp"
#include "../ComponentEditors/DefaultComponentEditors.hpp"
#include <entt/entt.hpp>

class InspectorWindow {
public:
    using DrawCustomFunction = std::function<void(entt::registry&, entt::entity)>;

    InspectorWindow(RegistryManager& registryManager, DrawCustomFunction drawCustomFunction);
    void DrawGui();
private:
    RegistryManager& registryManager;
    DefaultComponentEditors defaultComponentEditors;
    DrawCustomFunction drawCustomFunction;
};
