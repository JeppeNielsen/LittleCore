//
// Created by Codex on 02/05/2026.
//

#include "LocalTransformSceneDrawer.hpp"

using namespace LittleCore;

void LocalTransformSceneDrawer::Draw(SceneDrawerContext& context, LocalTransform& component) {
    (void)component;
    if (context.gizmoDrawer.IsSizableBoundsActive(context.objectEntity)) {
        return;
    }

    context.gizmoDrawer.DrawGizmo(context.gizmoContext,
                                  context.cameraRegistry,
                                  context.cameraEntity,
                                  context.objectRegistry,
                                  context.objectEntity,
                                  context.operation);
}
