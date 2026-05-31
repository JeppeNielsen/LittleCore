#include "ClickableRayCasterSystem.hpp"

#include "Camera.hpp"
#include "ClickableRaycaster.hpp"
#include "Input.hpp"
#include "WorldTransform.hpp"

using namespace LittleCore;

ClickableRayCasterSystem::ClickableRayCasterSystem(entt::registry& registry) : SystemBase(registry), clickableOctreeSystem(registry) {

}

bool ClickableRayCasterSystem::TryGetClosestEntityFromRay(Ray ray, entt::entity& closest, const Camera& camera, const WorldTransform& worldTransform) {
    closest = entt::null;

    std::vector<entt::entity> entities;

    clickableOctreeSystem.Query(ray, entities);

    if (entities.empty()) {
        return false;
    }

    std::sort(entities.begin(), entities.end(), [this, &camera, &worldTransform](entt::entity entityA, entt::entity entityB) {

        const WorldTransform& worldTransformA = registry.get<WorldTransform>(entityA);
        const float distanceA = camera.GetDistance(worldTransform.worldInverse, worldTransformA.world);

        const WorldTransform& worldTransformB = registry.get<WorldTransform>(entityB);
        const float distanceB = camera.GetDistance(worldTransform.worldInverse, worldTransformB.world);

        return distanceA > distanceB;
    });

    closest = entities[0];
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
        } else if (input.IsTouchUp({clickableRayCaster.buttonId})) {
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
            registry.patch<Clickable>(downEntity);
        } else {
            auto downEntity = clickableRayCaster.downEntity;
            auto& clickable = registry.get<Clickable>(downEntity);

            if (clickable.down || clickable.up || clickable.clicked) {
                clickable.down = false;
                clickable.up = false;
                clickable.clicked = false;
                registry.patch<Clickable>(downEntity);
            }

        }
    }






}

