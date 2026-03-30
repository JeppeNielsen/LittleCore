//
// Created by Jeppe Nielsen on 29/03/2026.
//

#pragma once
#include "SystemBase.hpp"
#include "Types.hpp"
#include "LocalTransform.hpp"

#include <glm/vec2.hpp>
#include <algorithm>
#include <cmath>


using namespace LittleCore;

struct MoverSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, velocity, transform]  : registry.view<const Velocity, LocalTransform>().each()) {
            transform.position += velocity.speed * dt;
            registry.patch<LocalTransform>(entity);
        }
    }
};

struct RotatorSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, rotator, transform]  : registry.view<const Rotater, LocalTransform>().each()) {
            transform.rotation *= quat({0,rotator.speed * dt ,0 });
            registry.patch<LocalTransform>(entity);
        }
    }

};

struct BobberSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[entity, bobber, transform]  : registry.view<Bobber, LocalTransform>().each()) {
            bobber.progress += bobber.speed * dt;
            transform.position.x = sin(bobber.progress) * bobber.amplitude;
            registry.patch<LocalTransform>(entity);
        }
    }

};

namespace {
    using vec2 = glm::vec2;

    struct CollisionResult {
        bool hit = false;
        vec2 response = vec2(0.0f);
    };

    CollisionResult circleVsRect(
            const vec2& circleCenter,
            float radius,
            const Rect& rect) {
        CollisionResult result;

        // Closest point on rectangle
        vec2 closest;
        closest.x = std::clamp(circleCenter.x, rect.min.x, rect.max.x);
        closest.y = std::clamp(circleCenter.y, rect.min.y, rect.max.y);

        vec2 delta = circleCenter - closest;
        float dist2 = glm::dot(delta, delta);
        float radius2 = radius * radius;

        // No collision
        if (dist2 >= radius2)
            return result;

        result.hit = true;

        // Normal case (circle center outside or touching edge)
        if (dist2 > 0.000001f) {
            float dist = std::sqrt(dist2);
            vec2 normal = delta / dist;
            float penetration = radius - dist;
            result.response = normal * penetration;
            return result;
        }

        // Special case: circle center inside rectangle
        float toLeft = circleCenter.x - rect.min.x;
        float toRight = rect.max.x - circleCenter.x;
        float toTop = circleCenter.y - rect.min.y;
        float toBottom = rect.max.y - circleCenter.y;

        float minDist = std::min(std::min(toLeft, toRight), std::min(toTop, toBottom));

        if (minDist == toLeft) {
            result.response = vec2(radius - toLeft, 0.0f);
        } else if (minDist == toRight) {
            result.response = vec2(-(radius - toRight), 0.0f);
        } else if (minDist == toTop) {
            result.response = vec2(0.0f, radius - toTop);
        } else {
            result.response = vec2(0.0f, -(radius - toBottom));
        }

        return result;
    }
}

struct CollisionSystem : LittleCore::SystemBase {

    void Update(float dt) {
        for(auto[ballEntity, ballTransform, velocity, ball]  : registry.view<LocalTransform, Velocity, Ball>().each()) {

            for(auto[batEntity, batTransform, bat]  : registry.view<LocalTransform, Bat>().each()) {

                Rect localRect = bat.bounds;
                localRect.min += (glm::vec2 )batTransform.position;
                localRect.max += (glm::vec2 )batTransform.position;

                auto result = circleVsRect(ballTransform.position, ball.radius, localRect);

                if (result.hit) {
                    velocity.speed = {
                        result.response.x,
                        result.response.y,
                    0};
                    batTransform.position += glm::vec3 (result.response.x, result.response.y, 0);
                    registry.patch<Velocity>(ballEntity);
                }



            }


        }


    }

};