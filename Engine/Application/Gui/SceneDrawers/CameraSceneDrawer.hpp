//
// Created by Codex on 02/05/2026.
//

#pragma once

#include "ComponentSceneDrawer.hpp"
#include "SceneDrawerContext.hpp"
#include "Camera.hpp"

namespace LittleCore {

    class CameraSceneDrawer : public ComponentSceneDrawer<Camera> {
    public:
        void Draw(SceneDrawerContext& context, Camera& component) override;
    };
}
