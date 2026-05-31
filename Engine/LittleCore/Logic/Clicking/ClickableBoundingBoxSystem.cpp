//
// Created by Jeppe Nielsen on 29/05/2026.
//

#include "ClickableBoundingBoxSystem.hpp"

#include "Clickable.hpp"
#include "LocalBoundingBox.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

ClickableBoundingBoxSystem::ClickableBoundingBoxSystem(entt::registry &registry) :
SystemBase(registry),
observer(registry, entt::collector.update<Sizable>().where<Clickable, LocalBoundingBox>()
                                         .group<Sizable, Clickable, LocalBoundingBox>())
{

}

void ClickableBoundingBoxSystem::Update() {
    for(auto entity : observer) {
        auto& localBoundingBox = registry.get<LocalBoundingBox>(entity);
        auto& sizable = registry.get<Sizable>(entity);
        localBoundingBox.bounds.center = {sizable.size.x * 0.5f,sizable.size.y * 0.5f,0.0f};
        localBoundingBox.bounds.extends = {sizable.size.x,sizable.size.y,0.0f};
        registry.patch<LocalBoundingBox>(entity);
    }
    observer.clear();
}