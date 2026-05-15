//
// Created by Jeppe Nielsen on 05/10/2025.
//

#include "SceneWindow.hpp"
#include "../EditorSimulations/EditorSimulation.hpp"
#include "../EditorSimulations/EditorCamera.hpp"
#include "imgui.h"
#include <cstdint>
#define SOKOL_IMGUI_NO_SOKOL_APP
#include <util/sokol_imgui.h>
#include "SokolDirect.hpp"
#include "CameraPicker.hpp"
#include "RegistryHelper.hpp"
#include "IgnoreSerialization.hpp"
#include "SceneDrawerContext.hpp"

using namespace LittleCore;

SceneWindow::SceneWindow(GameWindow& gameWindow) :
gameWindow(gameWindow)
{}

void SceneWindow::Draw(EditorSimulation& simulation) {
    for(auto& camera : simulation.cameraController.cameras) {
        DrawCamera(simulation, *camera.get());
    }
}

void RenderStats(RenderingStats& stats) {
    ImGui::Text("Entities: %i, DrawCalls: %i, Vertices: %i, Triangles: %i", stats.numEntities, stats.numRenderCalls, stats.numVertices, stats.numTriangles / 3);
}

void SceneWindow::DrawCamera(EditorSimulation& simulation, EditorCamera& camera) {
    bool isVisible = ImGui::Begin("Scene");

    if (!isVisible) {
        ImGui::End();
        return;
    }

    guiWindowInputController.Begin();



    ImVec2 windowSize = ImGui::GetContentRegionAvail();

    LocalTransform& localTransform = camera.simulation.registry.get<LocalTransform>(camera.cameraEntity);
    WorldTransform& cameraTransform = camera.simulation.registry.get<WorldTransform>(camera.cameraEntity);
    Camera& worldCamera = camera.simulation.registry.get<Camera>(camera.cameraEntity);

    simulation.context.renderer.screenSize = {windowSize.x, windowSize.y};
    if (windowSize.x > 32 && windowSize.y > 32) {
        camera.frameBuffer.Render((int) windowSize.x, (int) windowSize.y, [&]() {
            simulation.simulation.Render(0, cameraTransform, worldCamera, &simulation.context.renderer);
        });
    }

    if (lc_sg_valid(camera.frameBuffer.texture)) {
        ImGui::Image(static_cast<ImTextureID>(simgui_imtextureid(camera.frameBuffer.texture)), windowSize);
    }

    gizmoDrawer.Begin();


    ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE | ImGuizmo::OPERATION::ROTATE | ImGuizmo::OPERATION::SCALE;

    float gameViewAspect = gameWindow.Aspect();

    GizmoDrawerContext gizmoDrawerContext;

    for(auto selectedEntity : simulation.selection.selection) {
        if (!simulation.simulation.registry.valid(selectedEntity)) {
            continue;
        }
        SceneDrawerContext context {
            .gizmoDrawer = gizmoDrawer,
            .gizmoContext = gizmoDrawerContext,
            .cameraRegistry = camera.simulation.registry,
            .cameraEntity = camera.cameraEntity,
            .objectRegistry = simulation.simulation.registry,
            .objectEntity = selectedEntity,
            .operation = operation,
            .gameViewAspect = gameViewAspect
        };
        simulation.context.sceneDrawer->Draw(context);
    }

    bool gizmoClickStarted = gizmoDrawerContext.wasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left);

    if (gizmoDrawerContext.wasActive) {
        simulation.context.hierarchyChangedLastFrame = true;
    }

    if (!gizmoDrawerContext.wasActive) {
        guiWindowInputController.RunforSimulation(camera.simulation);

        for(auto[entity, cameraPicker] : camera.simulation.registry.view<CameraPicker>().each()) {
            if (cameraPicker.hasPicked) {
                simulation.selection.Clear();
                for (auto e: cameraPicker.pickedEntities) {
                    if (simulation.simulation.registry.any_of<IgnoreSerialization>(e)) {
                        e = RegistryHelper::FindParent(simulation.simulation.registry, e, [&](auto entity)->bool{
                            return !simulation.simulation.registry.any_of<IgnoreSerialization>(entity);
                        });
                    }
                    if (e != entt::null) {
                        simulation.selection.Select(e);
                    }
                }
            }
        }

    }

    RenderStats(simulation.context.renderer.stats);

    ImGui::End();

}
