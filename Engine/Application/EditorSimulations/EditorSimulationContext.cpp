//
// Created by Jeppe Nielsen on 06/10/2025.
//

#include "EditorSimulationContext.hpp"

using namespace LittleCore;

EditorSimulationContext::EditorSimulationContext(
        Renderer& renderer)
 : renderer(renderer),
   guiDrawer(nullptr),
   sceneDrawer(nullptr),
   hierarchyChangedLastFrame(false)
{

}
