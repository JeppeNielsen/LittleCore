//
// Created by Jeppe Nielsen on 18/10/2025.
//
#include "Mesh.hpp"


#pragma once
namespace LittleCore {

    class MeshLoader {
    public:
        void LoadMesh(const std::string& path, Mesh& mesh);
        bool IsValidMesh(const std::string& path);

    };

}
