//
// Created by Jeppe Nielsen on 17/10/2025.
//


#pragma once
#include "Mesh.hpp"
#include "MeshLoader.hpp"

namespace LittleCore {
    class MeshResourceLoader : public IResourceLoader<Mesh> {

        void Load(Mesh& resource) override;
        void Unload(Mesh& resource) override;
        bool IsLoaded() override;
        void Reload(Mesh& resource) override;

        MeshLoader meshLoader;
    };
}