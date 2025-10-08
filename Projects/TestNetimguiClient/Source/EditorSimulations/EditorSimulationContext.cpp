//
// Created by Jeppe Nielsen on 06/10/2025.
//

#include "EditorSimulationContext.hpp"

using namespace LittleCore;

EditorSimulationContext::EditorSimulationContext(
        Renderer& renderer,
        NetimguiClientController& netimguiClientController,
        EntityGuiDrawerBase& guiDrawer)
 : renderer(renderer),
   netimguiClientController(netimguiClientController),
   guiDrawer(guiDrawer)
{

}
