//
// Created by Jeppe Nielsen on 06/10/2025.
//


#pragma once

#include "EntityGuiDrawer.hpp"
#include "ComponentSceneDrawerCollection.hpp"
#include "Renderer.hpp"

namespace LittleCore {
    struct EditorSimulationContext {

        EditorSimulationContext(Renderer& renderer);

        Renderer& renderer;
        EntityGuiDrawerBase* guiDrawer;
        ComponentSceneDrawerBase* sceneDrawer;
        bool hierarchyChangedLastFrame;
    };
}
