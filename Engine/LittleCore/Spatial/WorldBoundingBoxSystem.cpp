//
//  WorldBoundingBoxSystem.cpp
//  Tiny
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "WorldBoundingBoxSystem.hpp"

using namespace LittleCore;

WorldBoundingBoxSystem::WorldBoundingBoxSystem(entt::registry &registry) :
    registry(registry),
    observer(registry, entt::collector.update<WorldTransform>().update<LocalBoundingBox>())

{ }

void WorldBoundingBoxSystem::Update() {

    for(auto entity : observer) {
        auto& localBoundingBox = registry.get<LocalBoundingBox>(entity);
        auto& worldTransform = registry.get<WorldTransform>(entity);
        auto& worldBoundingBox = registry.get<WorldBoundingBox>(entity);

        localBoundingBox.bounds.CreateWorldAligned(worldTransform.world, worldBoundingBox.bounds);
        registry.patch<WorldBoundingBox>(entity);
    }
    observer.clear();

}
