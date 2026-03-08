//
// Created by Jeppe Nielsen on 08/10/2025.
//

#include "GameWindow.hpp"
#include <imgui.h>
#include <cstdint>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include <util/sokol_imgui.h>
#include "SokolDirect.hpp"
#include "../EditorSimulations/EditorSimulation.hpp"
#include "ImguiInputHandler.hpp"

using namespace LittleCore;


GameWindow::GameWindow(NetimguiClientController& netimguiClientController) : netimguiClientController(netimguiClientController) {}

void GameWindow::Draw(EditorSimulation& simulation) {

    ImGui::Begin("Game");

    guiWindowInputController.Begin();
    guiWindowInputController.RunforSimulation(simulation.simulation);

    ImVec2 gameSize = ImGui::GetContentRegionAvail();

    simulation.context.renderer.screenSize = {gameSize.x, gameSize.y};
    if (gameSize.x>32 && gameSize.y>32) {
        frameBuffer.Render((int) gameSize.x, (int) gameSize.y, [&]() {
            simulation.simulation.Render(simulation.context.renderer);
        });
        netimguiClientController.SendTexture(frameBuffer.texture,  frameBuffer.width, frameBuffer.height);
    }

    if (lc_sg_valid(frameBuffer.texture)) {
        ImGui::Image(static_cast<ImTextureID>(simgui_imtextureid(frameBuffer.texture)), gameSize);
    }

    ImGui::End();

}

float GameWindow::Aspect() {
    return (float)frameBuffer.width / (float)frameBuffer.height;
}
