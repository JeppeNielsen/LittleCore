//
// Created by Codex on 03/05/2026.
//

#pragma once

#include "Math.hpp"

namespace LittleCore {
    struct StackLayout {
        enum class Direction {
            Horizontal,
            Vertical,
        };

        enum class Align {
            Start,
            Center,
            End,
            Stretch,
        };

        Direction direction = Direction::Horizontal;
        Align crossAlign = Align::Start;
        vec2 paddingMin = {0.0f, 0.0f};
        vec2 paddingMax = {0.0f, 0.0f};
        float spacing = 0.0f;
    };
}
