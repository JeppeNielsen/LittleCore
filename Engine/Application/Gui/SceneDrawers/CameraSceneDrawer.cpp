//
// Created by Codex on 02/05/2026.
//

#include "CameraSceneDrawer.hpp"

using namespace LittleCore;

void CameraSceneDrawer::Draw(SceneDrawerContext& context, Camera& component) {
    (void)component;
    context.gizmoDrawer.DrawCameraFrustum(context.cameraRegistry,
                                          context.cameraEntity,
                                          context.objectRegistry,
                                          context.objectEntity,
                                          context.gameViewAspect);
}
