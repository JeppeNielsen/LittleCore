//
// Created by Jeppe Nielsen on 15/01/2024.
//

#pragma once
#include <vector>

#include "Vertex.hpp"

namespace LittleCore {
    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> triangles;
    };
}
