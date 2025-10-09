//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "SceneWindow.hpp"
#include "../EditorSimulations/EditorSimulation.hpp"
#include "../EditorSimulations/EditorCamera.hpp"

#include "imgui.h"

using namespace LittleCore;

SceneWindow::SceneWindow(NetimguiClientController& netimguiClientController, GameWindow& gameWindow) :
netimguiClientController(netimguiClientController),
gameWindow(gameWindow) {}

void SceneWindow::Draw(EditorSimulation& simulation) {
    for(auto& camera : simulation.cameraController.cameras) {
        DrawCamera(simulation, *camera.get());
    }
}

void SceneWindow::DrawCamera(EditorSimulation& simulation, EditorCamera& camera) {
    ImGui::Begin("Scene");


    guiWindowInputController.RunforSimulation(camera.simulation);

    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    LocalTransform& localTransform = camera.simulation.registry.get<LocalTransform>(camera.cameraEntity);
    WorldTransform& cameraTransform = camera.simulation.registry.get<WorldTransform>(camera.cameraEntity);
    Camera& worldCamera = camera.simulation.registry.get<Camera>(camera.cameraEntity);

    glm::vec3 position = glm::vec3(cameraTransform.world[3]);
    std::cout << "x: " << position.x << ", y: " << position.y  << ", z: " << position.z  << "\n";

    simulation.context.renderer.screenSize = {windowSize.x, windowSize.y};
    if (windowSize.x > 32 && windowSize.y > 32) {
        camera.frameBuffer.Render((int) windowSize.x, (int) windowSize.y, [&]() {
            simulation.simulation.Render(0, cameraTransform, worldCamera, &simulation.context.renderer);
            bgfx::touch(0);
            bgfx::frame();
        });
        netimguiClientController.SendTexture(camera.frameBuffer.texture,  camera.frameBuffer.width, camera.frameBuffer.height);
    }

    ImGui::Image((void*)(uintptr_t)(camera.frameBuffer.texture.idx), windowSize);



    gizmoDrawer.Begin();


    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE;

    float gameViewAspect = gameWindow.Aspect();

    GizmoDrawerContext gizmoDrawerContext;

    for(auto selectedEntity : simulation.selection.selection) {
        if (!simulation.simulation.registry.valid(selectedEntity)) {
            continue;
        }
        gizmoDrawer.DrawGizmo(gizmoDrawerContext, camera.simulation.registry, camera.cameraEntity, simulation.simulation.registry, selectedEntity, operation);

        if (simulation.simulation.registry.all_of<Camera>(selectedEntity)) {
            gizmoDrawer.DrawCameraFrustum(camera.simulation.registry, camera.cameraEntity, simulation.simulation.registry, selectedEntity, gameViewAspect);
        }
    }

    bool gizmoClickStarted = gizmoDrawerContext.wasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    /*if (gizmoDrawerContext.wasHovered || gizmoDrawerContext.wasActive || gizmoClickStarted) {
        sdlInputHandler.handleDownEvents = false;
    }
     */

    ImGui::End();

}
