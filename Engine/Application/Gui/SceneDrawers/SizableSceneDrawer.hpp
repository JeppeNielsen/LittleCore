//
// Created by Codex on 03/05/2026.
//

#pragma once

#include "ComponentSceneDrawer.hpp"
#include "SceneDrawerContext.hpp"
#include "Sizable.hpp"

namespace LittleCore {

    class SizableSceneDrawer : public ComponentSceneDrawer<Sizable> {
    public:
        void Draw(SceneDrawerContext& context, Sizable& component) override;
    };
}
