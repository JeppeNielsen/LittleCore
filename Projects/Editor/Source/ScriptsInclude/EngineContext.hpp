//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include <functional>
#include "imgui.h"
#include "RegistryCollection.hpp"
#include "EditorRenderer.hpp"
#include "ResourceLoader.hpp"

class EngineContext {
public:
    ImGuiContext* imGuiContext;
    RegistryCollection* registryCollection;
    EditorRenderer* editorRenderer;
    ResourceLoader* resourceLoader;
};
