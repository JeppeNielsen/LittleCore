//
// Created by Jeppe Nielsen on 17/10/2025.
//

#include "MeshResourceLoader.hpp"
#include "MeshLoader.hpp"

using namespace LittleCore;
using namespace bgfx;

void MeshResourceLoader::Load(Mesh& resource) {
    MeshLoader::LoadMesh(path, resource);
}

void MeshResourceLoader::Unload(Mesh& resource) {
    resource.vertices.clear();
    resource.triangles.clear();
}

bool MeshResourceLoader::IsLoaded() {
    return true;
}

void MeshResourceLoader::Reload(Mesh &resource) {
    Unload(resource);
    Load(resource);
}
