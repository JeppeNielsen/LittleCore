//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "SceneView.hpp"
#include "../EditorSimulations/EditorSimulation.hpp"
#include "../EditorSimulations/EditorCamera.hpp"

#include "imgui.h"

using namespace LittleCore;

SceneView::SceneView(NetimguiClientController& netimguiClientController) : netimguiClientController(netimguiClientController) {}

void SceneView::Draw(EditorSimulation& simulation) {

    for(auto& camera : simulation.cameraController.cameras) {
        DrawCamera(simulation, *camera.get());
    }

}

void SceneView::DrawCamera(EditorSimulation& simulation, EditorCamera& camera) {
    ImGui::Begin("Scene");

    ImVec2 gameSize = ImGui::GetContentRegionAvail();

    WorldTransform& cameraTransform = camera.registry.get<WorldTransform>(camera.cameraEntity);
    Camera& worldCamera = camera.registry.get<Camera>(camera.cameraEntity);


    simulation.context.renderer.screenSize = {gameSize.x, gameSize.y};
    if (gameSize.x>32 && gameSize.y>32) {
        camera.frameBuffer.Render((int) gameSize.x, (int) gameSize.y, [&]() {
            simulation.simulation.Render(0, cameraTransform, worldCamera, &simulation.context.renderer);
            bgfx::touch(0);
            bgfx::frame();
        });
        netimguiClientController.SendTexture(camera.frameBuffer.texture,  camera.frameBuffer.width, camera.frameBuffer.height);
    }

    ImGui::Image((void*)(uintptr_t)(camera.frameBuffer.texture.idx), gameSize);

    ImVec2 gameViewMin = ImGui::GetItemRectMin();
    ImVec2 gameViewMax = ImGui::GetItemRectMax();

    gizmoDrawer.Begin();


    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE;

    GizmoDrawerContext gizmoDrawerContext;

    for(auto selectedEntity : simulation.selection.selection) {
        if (!simulation.simulation.registry.valid(selectedEntity)) {
            continue;
        }
        gizmoDrawer.DrawGizmo(gizmoDrawerContext, camera.registry, camera.cameraEntity, simulation.simulation.registry, selectedEntity, operation);

        if (simulation.simulation.registry.all_of<Camera>(selectedEntity)) {
            gizmoDrawer.DrawCameraFrustum(camera.registry, camera.cameraEntity, simulation.simulation.registry, selectedEntity, gameSize.x /gameSize.y);
        }
    }

    bool gizmoClickStarted = gizmoDrawerContext.wasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    /*if (gizmoDrawerContext.wasHovered || gizmoDrawerContext.wasActive || gizmoClickStarted) {
        sdlInputHandler.handleDownEvents = false;
    }
     */

    ImGui::End();

}
