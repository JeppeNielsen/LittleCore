#include "ClickableRayCasterSystem.hpp"

#include "Camera.hpp"
#include "ClickableRaycaster.hpp"
#include "Input.hpp"
#include "Sizable.hpp"
#include "WorldTransform.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace LittleCore;

namespace {
    bool TryIntersectSizableQuad(const Ray& worldRay, const WorldTransform& worldTransform, const Sizable& sizable) {
        Ray localRay = worldRay;
        localRay.Transform(worldTransform.worldInverse);

        constexpr float epsilon = 0.0001f;
        if (std::abs(localRay.direction.z) <= epsilon) {
            return false;
        }

        const float distance = -localRay.position.z / localRay.direction.z;
        if (distance < 0.0f) {
            return false;
        }

        const vec3 hitPosition = localRay.GetPosition(distance);
        const float minX = std::min(0.0f, sizable.size.x);
        const float maxX = std::max(0.0f, sizable.size.x);
        const float minY = std::min(0.0f, sizable.size.y);
        const float maxY = std::max(0.0f, sizable.size.y);

        return hitPosition.x >= minX &&
               hitPosition.x <= maxX &&
               hitPosition.y >= minY &&
               hitPosition.y <= maxY;
    }
}

ClickableRayCasterSystem::ClickableRayCasterSystem(entt::registry& registry) : SystemBase(registry), clickableOctreeSystem(registry) {

}

bool ClickableRayCasterSystem::TryGetClosestEntityFromRay(Ray ray, entt::entity& closest, const Camera& camera, const WorldTransform& worldTransform) {
    closest = entt::null;

    std::vector<entt::entity> entities;

    clickableOctreeSystem.Query(ray, entities);

    std::vector<entt::entity> intersectingEntities;
    intersectingEntities.reserve(entities.size());

    for (const entt::entity entity : entities) {
        const auto* sizable = registry.try_get<Sizable>(entity);
        if (!sizable) {
            continue;
        }

        const WorldTransform& entityWorldTransform = registry.get<WorldTransform>(entity);
        if (TryIntersectSizableQuad(ray, entityWorldTransform, *sizable)) {
            intersectingEntities.push_back(entity);
        }
    }

    if (intersectingEntities.empty()) {
        return false;
    }

    std::sort(intersectingEntities.begin(), intersectingEntities.end(), [this, &camera, &worldTransform](entt::entity entityA, entt::entity entityB) {

        const WorldTransform& worldTransformA = registry.get<WorldTransform>(entityA);
        const float distanceA = camera.GetDistance(worldTransform.worldInverse, worldTransformA.world);

        const WorldTransform& worldTransformB = registry.get<WorldTransform>(entityB);
        const float distanceB = camera.GetDistance(worldTransform.worldInverse, worldTransformB.world);

        return distanceA < distanceB;
    });

    closest = intersectingEntities[0];
    return true;
}

void ClickableRayCasterSystem::Update() {
    clickableOctreeSystem.Update();

    for(auto[entity, worldTransform, camera, input, clickableRayCaster] : registry.view<const WorldTransform, const Camera, const Input, ClickableRaycaster>().each()) {
        if (!registry.valid(clickableRayCaster.downEntity)) {
            if (input.IsTouchDown({clickableRayCaster.buttonId})) {
                ivec2 screenPos = input.touchPosition[0].position;
                auto ray = camera.GetRay(worldTransform, input.screenSize, screenPos);

                entt::entity closest;
                if (!TryGetClosestEntityFromRay(ray, closest, camera, worldTransform)) {
                    continue;
                }

                clickableRayCaster.downEntity = closest;
                auto& clickable = registry.get<Clickable>(closest);
                clickable.down = true;
                registry.patch<Clickable>(closest);
            }
        } else {

            auto downEntity = clickableRayCaster.downEntity;
            auto& clickable = registry.get<Clickable>(downEntity);
            bool wasChanged = false;

            if (clickable.down || clickable.up || clickable.clicked) {

                if (clickable.up) {
                    clickableRayCaster.downEntity = entt::null;
                }

                clickable.down = false;
                clickable.up = false;
                clickable.clicked = false;
                wasChanged = true;
            }

            if (input.IsTouchUp({clickableRayCaster.buttonId})) {
                ivec2 screenPos = input.touchPosition[0].position;
                auto ray = camera.GetRay(worldTransform, input.screenSize, screenPos);

                auto downEntity = clickableRayCaster.downEntity;
                auto& clickable = registry.get<Clickable>(downEntity);

                bool didChange = false;
                entt::entity closest;
                if (TryGetClosestEntityFromRay(ray, closest, camera, worldTransform) && closest == downEntity) {
                    clickable.clicked = true;
                    didChange = true;
                }

                clickable.up = true;
                wasChanged = true;
            }

            if (wasChanged) {
                registry.patch<Clickable>(downEntity);
            }

        }
    }






}
