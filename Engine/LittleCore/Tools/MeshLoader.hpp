//
// Created by Jeppe Nielsen on 18/10/2025.
//
#include "Mesh.hpp"


#pragma once
namespace LittleCore {

    class MeshLoader {
    public:
        static void LoadMesh(const std::string& path, Mesh& mesh);
        static bool IsValidMesh(const std::string& path);

    };

}
