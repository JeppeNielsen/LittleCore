//
// Created by Jeppe Nielsen on 23/10/2025.
//
#pragma once
#include <string>

namespace LittleCore {
    struct RenderingStats {
        int numRenderCalls = 0;
        int numEntities = 0;
        int numVertices = 0;
        int numTriangles = 0;
    };

}