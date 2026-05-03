//
// Created by Codex on 03/05/2026.
//

#pragma once

#include "Math.hpp"

namespace LittleCore {
    struct LayoutElement {
        vec2 min = {0.0f, 0.0f};
        vec2 preferred = {0.0f, 0.0f};
        vec2 max = {0.0f, 0.0f};
        vec2 grow = {0.0f, 0.0f};
        vec2 shrink = {1.0f, 1.0f};
        bool ignoreLayout = false;
    };
}
