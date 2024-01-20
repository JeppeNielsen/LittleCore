//
// Created by Jeppe Nielsen on 18/01/2024.
//

#include "WorldTransformSystem.hpp"

#include <Hierarchy.hpp>

using namespace LittleCore;

WorldTransformSystem::WorldTransformSystem(entt::registry& registry) :
registry(registry),
observer(registry, entt::collector.update<LocalTransform>().update<Hierarchy>().update<WorldTransform>()) {

}

void CalculateParentMatrices(entt::registry& registry, entt::entity entity, std::unordered_set<entt::entity>& dirtyEntities) {
    if (dirtyEntities.find(entity) == dirtyEntities.end()) {
        return;
    }
    dirtyEntities.erase(entity);

    auto& hierarchy = registry.get<Hierarchy>(entity);
    auto& localTransform = registry.get<LocalTransform>(entity);
    auto& worldTransform = registry.get<WorldTransform>(entity);

    if (!registry.valid(hierarchy.parent)) {
        worldTransform.world = localTransform.GetLocalToParent();
    } else {
        CalculateParentMatrices(registry, hierarchy.parent, dirtyEntities);
        auto& parentWorldTransform = registry.get<WorldTransform>(hierarchy.parent);
        worldTransform.world = parentWorldTransform.world * localTransform.GetLocalToParent();
    }
    worldTransform.worldInverse = glm::inverse(worldTransform.world);
}

void CalculateChildMatrices(entt::registry& registry, entt::entity entity, std::unordered_set<entt::entity>& dirtyEntities) {

    auto& hierarchy = registry.get<Hierarchy>(entity);
    auto& localTransform = registry.get<LocalTransform>(entity);
    auto& worldTransform = registry.get<WorldTransform>(entity);

    if (!registry.valid(hierarchy.parent)) {
        worldTransform.world = localTransform.GetLocalToParent();
    } else {
        auto& parentWorldTransform = registry.get<WorldTransform>(hierarchy.parent);
        worldTransform.world = parentWorldTransform.world * localTransform.GetLocalToParent();
    }

    worldTransform.worldInverse = glm::inverse(worldTransform.world);

    for (auto child: hierarchy.children) {
        if (dirtyEntities.find(child) != dirtyEntities.end()) {
            continue;
        }
        CalculateChildMatrices(registry, child, dirtyEntities);
    }
}

void WorldTransformSystem::Update() {
    if (observer.empty()) {
        return;
    }

    std::unordered_set<entt::entity> dirtyEntities;
    dirtyEntities.insert(observer.begin(), observer.end());

    for (auto localTransformEntity : observer) {
        CalculateParentMatrices(registry, localTransformEntity, dirtyEntities);
    }

    dirtyEntities.insert(observer.begin(), observer.end());

    for (auto localTransformEntity : observer) {
        CalculateChildMatrices(registry, localTransformEntity, dirtyEntities);
    }

    observer.clear();
}