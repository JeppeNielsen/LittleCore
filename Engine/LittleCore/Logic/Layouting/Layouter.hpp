//
// Created by Codex on 03/05/2026.
//

#pragma once

#include "Math.hpp"

namespace LittleCore {
    struct Layouter {
        enum class LayoutMode {
            None,
            Horizontal,
            Vertical,
        };

        LayoutMode childrenLayoutMode = LayoutMode::None;
        vec2 min = {0.0f, 0.0f};
        vec2 desired = {0.0f, 0.0f};
        vec2 max = {0.0f, 0.0f};
    };
}
