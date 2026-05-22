//
// Created by Codex on 03/05/2026.
//

#pragma once

#include "Math.hpp"

namespace LittleCore {
    struct Layouter {
        // RectTransform-style self layout.
        vec2 anchorMin = {0.0f, 0.0f};
        vec2 anchorMax = {0.0f, 0.0f};
        vec2 anchoredPosition = {0.0f, 0.0f};
        vec2 sizeDelta = {0.0f, 0.0f};
        vec2 pivot = {0.5f, 0.5f};
    };
}
