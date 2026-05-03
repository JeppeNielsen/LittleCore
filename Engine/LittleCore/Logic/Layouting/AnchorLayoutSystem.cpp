//
// Created by Codex on 03/05/2026.
//

#include "AnchorLayoutSystem.hpp"
#include "Anchors.hpp"
#include "Hierarchy.hpp"
#include "LocalTransform.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

AnchorLayoutSystem::AnchorLayoutSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Anchors>().group<Anchors, Sizable, LocalTransform, Hierarchy>()
                .update<Sizable>().where<Anchors, LocalTransform, Hierarchy>()
                .update<Hierarchy>().where<Anchors, Sizable, LocalTransform>()),
        isDirty(true) {
    registry.on_construct<Anchors>().connect<&AnchorLayoutSystem::MarkDirty>(this);
    registry.on_construct<Sizable>().connect<&AnchorLayoutSystem::MarkDirty>(this);
    registry.on_construct<LocalTransform>().connect<&AnchorLayoutSystem::MarkDirty>(this);
    registry.on_construct<Hierarchy>().connect<&AnchorLayoutSystem::MarkDirty>(this);
}

void AnchorLayoutSystem::Update() {
    if (!observer.empty()) {
        isDirty = true;
        observer.clear();
    }

    if (!isDirty) {
        return;
    }

    auto view = registry.view<Anchors, Sizable, LocalTransform, Hierarchy>();
    for (auto entity : view) {
        ApplyAnchors(entity);
    }

    isDirty = false;
}

void AnchorLayoutSystem::MarkDirty(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) {
        isDirty = true;
    }
}

void AnchorLayoutSystem::ApplyAnchors(entt::entity entity) {
    auto& anchors = registry.get<Anchors>(entity);
    auto& sizable = registry.get<Sizable>(entity);
    auto& transform = registry.get<LocalTransform>(entity);
    const auto& hierarchy = registry.get<Hierarchy>(entity);

    if (!registry.valid(hierarchy.parent) || !registry.all_of<Sizable>(hierarchy.parent)) {
        return;
    }

    const vec2 parentSize = registry.get<Sizable>(hierarchy.parent).size;

    const vec2 anchorMin = anchors.min * parentSize + anchors.offsetMin;
    const vec2 anchorMax = anchors.max * parentSize + anchors.offsetMax;

    const vec2 newSize = {
            std::max(0.0f, anchorMax.x - anchorMin.x),
            std::max(0.0f, anchorMax.y - anchorMin.y)
    };

    if (sizable.size != newSize) {
        sizable.size = newSize;
        registry.patch<Sizable>(entity);
    }

    const vec3 newPosition = {anchorMin.x, anchorMin.y, transform.position.z};
    if (transform.position != newPosition) {
        transform.position = newPosition;
        registry.patch<LocalTransform>(entity);
    }
}
