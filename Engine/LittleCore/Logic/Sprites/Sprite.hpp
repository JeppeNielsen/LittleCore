//
// Created by Jeppe Nielsen on 23/05/2026.
//

#pragma once
#include "Math.hpp"

namespace LittleCore {
    struct Sprite {
        // UV cut percentages measured from each edge.
        float slicingLeft = 0.0f;
        float slicingRight = 0.0f;
        float slicingTop = 0.0f;
        float slicingBottom = 0.0f;

        // Output corner width/height used when generating a 9-slice mesh.
        glm::vec2 cornerSize = {0.0f, 0.0f};
    };

}
