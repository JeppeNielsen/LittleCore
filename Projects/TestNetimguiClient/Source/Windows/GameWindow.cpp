//
// Created by Jeppe Nielsen on 08/10/2025.
//

#include "GameWindow.hpp"
#include <imgui.h>
#include "../EditorSimulations/EditorSimulation.hpp"
#include "ImguiInputHandler.hpp"

using namespace LittleCore;


GameWindow::GameWindow(NetimguiClientController& netimguiClientController) : netimguiClientController(netimguiClientController) {}

void GameWindow::Draw(EditorSimulation& simulation) {

    ImGui::Begin("Game");

    guiWindowInputController.RunforSimulation(simulation.simulation);

    ImVec2 gameSize = ImGui::GetContentRegionAvail();

    simulation.context.renderer.screenSize = {gameSize.x, gameSize.y};
    if (gameSize.x>32 && gameSize.y>32) {
        frameBuffer.Render((int) gameSize.x, (int) gameSize.y, [&]() {
            simulation.simulation.Render(simulation.context.renderer);
            bgfx::touch(0);
            bgfx::frame();
        });
        netimguiClientController.SendTexture(frameBuffer.texture,  frameBuffer.width, frameBuffer.height);
    }

    ImGui::Image((void*)(uintptr_t)(frameBuffer.texture.idx), gameSize);

    ImGui::End();

}

float GameWindow::Aspect() {
    return (float)frameBuffer.width / (float)frameBuffer.height;
}
