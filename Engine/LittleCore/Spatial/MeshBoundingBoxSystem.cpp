//
//  MeshBoundingBoxSystem.cpp
//  TinyOSX
//
//  Created by Jeppe Nielsen on 24/09/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "MeshBoundingBoxSystem.hpp"


using namespace LittleCore;

/*
void MeshBoundingBoxSystem::Changed(const Mesh &mesh, LocalBoundingBox &localBoundingBox) {
}

void MeshBoundingBoxSystem::Update(const Mesh &mesh, LocalBoundingBox &localBoundingBox) {
    
    if (mesh.vertices.empty()) {
        localBoundingBox.bounds.center = {0.0f,0.0f,0.0f};
        localBoundingBox.bounds.extends = {0.0f,0.0f,0.0f};
    }
    
    vec3 min = mesh.vertices[0].Position;
    vec3 max = min;
    
    for (int i = 1; i<mesh.vertices.size(); ++i) {
        auto& pos = mesh.vertices[i].Position;
        min = glm::min(min, pos);
        max = glm::max(max, pos);
    }
    
    localBoundingBox.bounds.center = (min + max) * 0.5f;
    localBoundingBox.bounds.extends = (max - min) * 0.5f;
}

 */