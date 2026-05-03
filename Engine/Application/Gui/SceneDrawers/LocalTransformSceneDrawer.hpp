//
// Created by Codex on 02/05/2026.
//

#pragma once

#include "ComponentSceneDrawer.hpp"
#include "SceneDrawerContext.hpp"
#include "LocalTransform.hpp"

namespace LittleCore {

    class LocalTransformSceneDrawer : public ComponentSceneDrawer<LocalTransform> {
    public:
        void Draw(SceneDrawerContext& context, LocalTransform& component) override;
    };
}
