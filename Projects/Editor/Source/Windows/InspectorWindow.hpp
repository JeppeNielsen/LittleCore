//
// Created by Jeppe Nielsen on 01/01/2025.
//


#pragma once
#include <functional>
#include "../ScriptsInclude/RegistryManager.hpp"
#include "../ComponentEditors/DefaultComponentEditors.hpp"
#include <entt/entt.hpp>
#include "../ScriptsInclude/ComponentDrawer.hpp"
#include "../ScriptsInclude/ComponentFactory.hpp"

class InspectorWindow {
public:
    InspectorWindow(RegistryManager& registryManager,
                    ComponentDrawer& componentDrawer,
                    ComponentFactory& componentFactory);
    void DrawGui();
private:
    RegistryManager& registryManager;
    DefaultComponentEditors defaultComponentEditors;
    ComponentDrawer& componentDrawer;
    ComponentFactory& componentFactory;
};
