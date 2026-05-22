//
// Created by Codex on 03/05/2026.
//

#include "LayoutSystem.hpp"

#include <algorithm>
#include <vector>

#include "Hierarchy.hpp"
#include "Layouter.hpp"
#include "LocalTransform.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

namespace {
    struct LayoutEntity {
        entt::entity entity;
        int depth;
    };

    bool HasDefaultRectLayout(const Layouter& layouter) {
        return layouter.anchorMin == vec2(0.0f, 0.0f) &&
               layouter.anchorMax == vec2(0.0f, 0.0f) &&
               layouter.anchoredPosition == vec2(0.0f, 0.0f) &&
               layouter.sizeDelta == vec2(0.0f, 0.0f) &&
               layouter.pivot == vec2(0.5f, 0.5f);
    }

    int CalculateDepth(const entt::registry& registry, const entt::entity entity) {
        int depth = 0;
        const Hierarchy* hierarchy = registry.try_get<Hierarchy>(entity);
        while (hierarchy && registry.valid(hierarchy->parent)) {
            ++depth;
            hierarchy = registry.try_get<Hierarchy>(hierarchy->parent);
        }
        return depth;
    }
}

LayoutSystem::LayoutSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Layouter>().group<Layouter, Sizable, LocalTransform, Hierarchy>()
                .update<Sizable>().where<Layouter, LocalTransform, Hierarchy>()
                .update<LocalTransform>().where<Layouter, Sizable, Hierarchy>()
                .update<Hierarchy>().group<Hierarchy>()),
        isDirty(true) {
    registry.on_construct<Layouter>().connect<&LayoutSystem::MarkDirty>(this);
    registry.on_construct<Sizable>().connect<&LayoutSystem::MarkDirty>(this);
    registry.on_construct<LocalTransform>().connect<&LayoutSystem::MarkDirty>(this);
    registry.on_construct<Hierarchy>().connect<&LayoutSystem::MarkDirty>(this);

    registry.on_destroy<Layouter>().connect<&LayoutSystem::MarkDirtyOnDestroy>(this);
    registry.on_destroy<Sizable>().connect<&LayoutSystem::MarkDirtyOnDestroy>(this);
    registry.on_destroy<LocalTransform>().connect<&LayoutSystem::MarkDirtyOnDestroy>(this);
    registry.on_destroy<Hierarchy>().connect<&LayoutSystem::MarkDirtyOnDestroy>(this);
}

void LayoutSystem::InitializeLayouter(entt::entity entity) {
    if (!registry.all_of<Layouter, Sizable, LocalTransform>(entity)) {
        return;
    }

    auto& layouter = registry.get<Layouter>(entity);
    if (!HasDefaultRectLayout(layouter)) {
        return;
    }

    const auto& sizable = registry.get<Sizable>(entity);
    const auto& transform = registry.get<LocalTransform>(entity);

    if (sizable.size == vec2(0.0f, 0.0f) &&
        transform.position.x == 0.0f &&
        transform.position.y == 0.0f) {
        return;
    }

    layouter.anchorMin = {0.0f, 0.0f};
    layouter.anchorMax = {0.0f, 0.0f};
    layouter.sizeDelta = sizable.size;
    layouter.pivot = {0.5f, 0.5f};
    layouter.anchoredPosition = vec2(transform.position) + sizable.size * 0.5f;
}

void LayoutSystem::Update() {
    if (!observer.empty()) {
        isDirty = true;
        observer.clear();
    }

    if (!isDirty) {
        return;
    }

    std::vector<LayoutEntity> layoutEntities;
    auto layouterView = registry.view<Layouter, Sizable, LocalTransform, Hierarchy>();
    layoutEntities.reserve(layouterView.size_hint());
    for (auto entity : layouterView) {
        InitializeLayouter(entity);
        layoutEntities.push_back({entity, CalculateDepth(registry, entity)});
    }

    std::sort(layoutEntities.begin(), layoutEntities.end(), [](const LayoutEntity& a, const LayoutEntity& b) {
        return a.depth < b.depth;
    });

    for (const auto& layoutEntity : layoutEntities) {
        ApplyRectLayout(layoutEntity.entity);
    }

    isDirty = false;
}

void LayoutSystem::MarkDirty(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) {
        isDirty = true;
    }
}

void LayoutSystem::MarkDirtyOnDestroy(entt::registry& registry, entt::entity entity) {
    (void)registry;
    (void)entity;
    isDirty = true;
}

void LayoutSystem::ApplyRectLayout(entt::entity entity) {
    auto& layouter = registry.get<Layouter>(entity);
    const auto& hierarchy = registry.get<Hierarchy>(entity);
    if (!registry.valid(hierarchy.parent) || !registry.all_of<Sizable>(hierarchy.parent)) {
        return;
    }

    auto& sizable = registry.get<Sizable>(entity);
    auto& transform = registry.get<LocalTransform>(entity);
    const vec2 parentSize = registry.get<Sizable>(hierarchy.parent).size;
    const vec2 anchorRectMin = layouter.anchorMin * parentSize;
    const vec2 anchorRectMax = layouter.anchorMax * parentSize;
    const vec2 anchorSpan = anchorRectMax - anchorRectMin;
    const vec2 newSize = glm::max(anchorSpan + layouter.sizeDelta, vec2(0.0f, 0.0f));
    const vec2 anchorReference = anchorRectMin + anchorSpan * layouter.pivot;
    const vec2 rectMin = anchorReference + layouter.anchoredPosition - layouter.pivot * newSize;

    if (sizable.size != newSize) {
        sizable.size = newSize;
        registry.patch<Sizable>(entity);
    }

    vec3 newPosition = transform.position;
    newPosition.x = rectMin.x;
    newPosition.y = rectMin.y;

    if (transform.position != newPosition) {
        transform.position = newPosition;
        registry.patch<LocalTransform>(entity);
    }
}
