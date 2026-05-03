//
// Created by Jeppe Nielsen on 06/10/2025.
//


#pragma once

#include "EntityGuiDrawer.hpp"
#include "ComponentSceneDrawerCollection.hpp"
#include "Renderer.hpp"
#include "NetimguiClientController.hpp"

namespace LittleCore {
    struct EditorSimulationContext {

        EditorSimulationContext(Renderer& renderer, NetimguiClientController& netimguiClientController);

        Renderer& renderer;
        NetimguiClientController& netimguiClientController;
        EntityGuiDrawerBase* guiDrawer;
        ComponentSceneDrawerBase* sceneDrawer;
        bool hierarchyChangedLastFrame;
    };
}
