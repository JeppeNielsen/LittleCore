//
//  MeshBoundingBoxSystem.cpp
//  TinyOSX
//
//  Created by Jeppe Nielsen on 24/09/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "MeshBoundingBoxSystem.hpp"


using namespace LittleCore;

MeshBoundingBoxSystem::MeshBoundingBoxSystem(entt::registry &registry) :
registry(registry),
observer(registry, entt::collector.update<Mesh>().where<LocalBoundingBox>().group<Mesh, LocalBoundingBox>())
{

}

void MeshBoundingBoxSystem::Update() {
    for(auto entity : observer) {
        auto& localBoundingBox = registry.get<LocalBoundingBox>(entity);
        auto& mesh = registry.get<Mesh>(entity);

        if (mesh.vertices.empty()) {
            localBoundingBox.bounds.center = {0.0f,0.0f,0.0f};
            localBoundingBox.bounds.extends = {0.0f,0.0f,0.0f};
            continue;
        }

        vec3 min = mesh.vertices[0].position;
        vec3 max = min;

        for (int i = 1; i<mesh.vertices.size(); ++i) {
            auto& pos = mesh.vertices[i].position;
            min = glm::min(min, pos);
            max = glm::max(max, pos);
        }

        localBoundingBox.bounds.center = (min + max) * 0.5f;
        localBoundingBox.bounds.extends = (max - min) * 0.5f;

        registry.patch<LocalBoundingBox>(entity);
    }

    observer.clear();
}
