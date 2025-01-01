//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include <functional>
#include "imgui.h"
#include "EditorRenderer.hpp"
#include "ResourceLoader.hpp"
#include "RegistryManager.hpp"
#include "ComponentDrawer.hpp"

class EngineContext {
public:
    ImGuiContext* imGuiContext;
    RegistryManager* registryManager;
    EditorRenderer* editorRenderer;
    ResourceLoader* resourceLoader;
    ComponentDrawer* componentDrawer;
};
