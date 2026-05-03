//
// Created by Codex on 03/05/2026.
//

#include "SizableSceneDrawer.hpp"

using namespace LittleCore;

void SizableSceneDrawer::Draw(SceneDrawerContext& context, Sizable& component) {
    if (context.gizmoContext.wasActive && !context.gizmoDrawer.IsSizableBoundsActive(context.objectEntity)) {
        return;
    }

    context.gizmoDrawer.DrawSizableBounds(context.gizmoContext,
                                          context.cameraRegistry,
                                          context.cameraEntity,
                                          context.objectRegistry,
                                          context.objectEntity,
                                          component);
}
