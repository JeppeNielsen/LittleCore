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
    struct LayoutChild {
        entt::entity entity;
        LittleCore::Layouter* layouter;
        LittleCore::Sizable* sizable;
        LittleCore::LocalTransform* transform;
    };

    struct LayoutRange {
        float min;
        float desired;
        float max;
    };

    LayoutRange SanitizeRange(float minValue, float desiredValue, float maxValue) {
        desiredValue = std::max(desiredValue, minValue);
        maxValue = std::max(maxValue, desiredValue);
        return {minValue, desiredValue, maxValue};
    }

    float ResolveAlongAxis(float available, float totalMin, float totalDesired, float totalMax, LayoutRange range) {
        if (available <= totalMin || totalDesired <= totalMin) {
            return range.min;
        }

        if (available <= totalDesired) {
            const float t = (available - totalMin) / (totalDesired - totalMin);
            return glm::mix(range.min, range.desired, t);
        }

        if (available <= totalMax && totalMax > totalDesired) {
            const float t = (available - totalDesired) / (totalMax - totalDesired);
            return glm::mix(range.desired, range.max, t);
        }

        return range.max;
    }

    float ResolveStandaloneAxis(float available, LayoutRange range) {
        return ResolveAlongAxis(available, range.min, range.desired, range.max, range);
    }
}

LayoutSystem::LayoutSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Layouter>().group<Layouter, Sizable, Hierarchy>()
                .update<Sizable>().where<Layouter>()
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

void LayoutSystem::Update() {
    if (!observer.empty()) {
        isDirty = true;
        observer.clear();
    }

    if (!isDirty) {
        return;
    }

    auto view = registry.view<Layouter, Sizable, Hierarchy>();
    for (auto entity : view) {
        auto& layouter = view.template get<Layouter>(entity);
        if (layouter.childrenLayoutMode == Layouter::LayoutMode::None) {
            continue;
        }
        if (HasLayoutParent(entity)) {
            continue;
        }
        ApplyLayout(entity);
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

bool LayoutSystem::HasLayoutParent(entt::entity entity) const {
    const auto* hierarchy = registry.try_get<Hierarchy>(entity);
    if (!hierarchy || !registry.valid(hierarchy->parent)) {
        return false;
    }

    const auto* parentLayouter = registry.try_get<Layouter>(hierarchy->parent);
    return parentLayouter &&
           parentLayouter->childrenLayoutMode != Layouter::LayoutMode::None &&
           registry.all_of<Sizable, Hierarchy>(hierarchy->parent);
}

void LayoutSystem::ApplyLayout(entt::entity entity) {
    auto& layouter = registry.get<Layouter>(entity);
    const vec2 parentSize = registry.get<Sizable>(entity).size;
    const auto& hierarchy = registry.get<Hierarchy>(entity);

    std::vector<LayoutChild> children;
    children.reserve(hierarchy.children.size());
    for (auto child : hierarchy.children) {
        auto* childLayouter = registry.try_get<Layouter>(child);
        auto* childSizable = registry.try_get<Sizable>(child);
        auto* childTransform = registry.try_get<LocalTransform>(child);
        if (!childLayouter || !childSizable || !childTransform) {
            continue;
        }
        children.push_back({child, childLayouter, childSizable, childTransform});
    }

    if (children.empty()) {
        return;
    }

    if (layouter.childrenLayoutMode == Layouter::LayoutMode::Horizontal) {
        float totalMin = 0.0f;
        float totalDesired = 0.0f;
        float totalMax = 0.0f;
        std::vector<LayoutRange> ranges(children.size());
        std::vector<LayoutRange> crossRanges(children.size());

        for (size_t i = 0; i < children.size(); ++i) {
            ranges[i] = SanitizeRange(children[i].layouter->min.x, children[i].layouter->desired.x, children[i].layouter->max.x);
            crossRanges[i] = SanitizeRange(children[i].layouter->min.y, children[i].layouter->desired.y, children[i].layouter->max.y);
            totalMin += ranges[i].min;
            totalDesired += ranges[i].desired;
            totalMax += ranges[i].max;
        }

        float cursor = 0.0f;
        for (size_t i = 0; i < children.size(); ++i) {
            const float width = ResolveAlongAxis(parentSize.x, totalMin, totalDesired, totalMax, ranges[i]);
            const float height = ResolveStandaloneAxis(parentSize.y, crossRanges[i]);
            const vec2 newSize = {width, height};
            vec3 newPosition = children[i].transform->position;
            newPosition.x = cursor;
            newPosition.y = (parentSize.y - height) * 0.5f;

            if (children[i].sizable->size != newSize) {
                children[i].sizable->size = newSize;
                registry.patch<Sizable>(children[i].entity);
            }

            if (children[i].transform->position != newPosition) {
                children[i].transform->position = newPosition;
                registry.patch<LocalTransform>(children[i].entity);
            }

            cursor += width;

            if (children[i].layouter->childrenLayoutMode != Layouter::LayoutMode::None && registry.all_of<Hierarchy>(children[i].entity)) {
                ApplyLayout(children[i].entity);
            }
        }
        return;
    }

    float totalMin = 0.0f;
    float totalDesired = 0.0f;
    float totalMax = 0.0f;
    std::vector<LayoutRange> ranges(children.size());
    std::vector<LayoutRange> crossRanges(children.size());

    for (size_t i = 0; i < children.size(); ++i) {
        ranges[i] = SanitizeRange(children[i].layouter->min.y, children[i].layouter->desired.y, children[i].layouter->max.y);
        crossRanges[i] = SanitizeRange(children[i].layouter->min.x, children[i].layouter->desired.x, children[i].layouter->max.x);
        totalMin += ranges[i].min;
        totalDesired += ranges[i].desired;
        totalMax += ranges[i].max;
    }

    float cursor = parentSize.y;
    for (size_t i = 0; i < children.size(); ++i) {
        const float height = ResolveAlongAxis(parentSize.y, totalMin, totalDesired, totalMax, ranges[i]);
        const float width = ResolveStandaloneAxis(parentSize.x, crossRanges[i]);
        const vec2 newSize = {width, height};
        cursor -= height;

        vec3 newPosition = children[i].transform->position;
        newPosition.x = (parentSize.x - width) * 0.5f;
        newPosition.y = cursor;

        if (children[i].sizable->size != newSize) {
            children[i].sizable->size = newSize;
            registry.patch<Sizable>(children[i].entity);
        }

        if (children[i].transform->position != newPosition) {
            children[i].transform->position = newPosition;
            registry.patch<LocalTransform>(children[i].entity);
        }

        if (children[i].layouter->childrenLayoutMode != Layouter::LayoutMode::None && registry.all_of<Hierarchy>(children[i].entity)) {
            ApplyLayout(children[i].entity);
        }
    }
}
