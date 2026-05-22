//
// Created by Codex on 03/05/2026.
//

#include "StackLayoutSystem.hpp"
#include "Hierarchy.hpp"
#include "LayoutElement.hpp"
#include "LocalTransform.hpp"
#include "Sizable.hpp"
#include "StackLayout.hpp"

#include <algorithm>
#include <vector>

using namespace LittleCore;

namespace {
    struct StackChild {
        entt::entity entity;
        LittleCore::LayoutElement* element;
        LittleCore::Sizable* sizable;
        LittleCore::LocalTransform* transform;
    };

    float ResolveAlongAxis(float available, float minValue, float preferredValue, float maxValue) {
        if (available <= minValue) {
            return minValue;
        }
        if (available <= preferredValue) {
            return available;
        }
        return std::min(available, maxValue);
    }
}

StackLayoutSystem::StackLayoutSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<StackLayout>().group<StackLayout, Sizable, Hierarchy>()
                .update<LayoutElement>().where<Sizable, LocalTransform>()
                .update<Sizable>().group<Sizable, LocalTransform>()
                .update<Hierarchy>().group<Hierarchy>()),
        isDirty(true) {
    registry.on_construct<StackLayout>().connect<&StackLayoutSystem::MarkDirty>(this);
    registry.on_construct<LayoutElement>().connect<&StackLayoutSystem::MarkDirty>(this);
    registry.on_construct<Sizable>().connect<&StackLayoutSystem::MarkDirty>(this);
    registry.on_construct<LocalTransform>().connect<&StackLayoutSystem::MarkDirty>(this);
    registry.on_construct<Hierarchy>().connect<&StackLayoutSystem::MarkDirty>(this);
}

void StackLayoutSystem::Update() {
    if (!observer.empty()) {
        isDirty = true;
        observer.clear();
    }

    if (!isDirty) {
        return;
    }

    auto view = registry.view<StackLayout, Sizable, Hierarchy>();
    for (auto entity : view) {
        if (HasStackLayoutParent(entity)) {
            continue;
        }
        ApplyLayout(entity);
    }

    isDirty = false;
}

void StackLayoutSystem::MarkDirty(entt::registry& registry, entt::entity entity) {
    if (registry.valid(entity)) {
        isDirty = true;
    }
}

bool StackLayoutSystem::HasStackLayoutParent(entt::entity entity) const {
    const auto* hierarchy = registry.try_get<Hierarchy>(entity);
    return hierarchy && registry.valid(hierarchy->parent) && registry.all_of<StackLayout, Sizable, Hierarchy>(hierarchy->parent);
}

void StackLayoutSystem::ApplyLayout(entt::entity entity) {
    auto& layout = registry.get<StackLayout>(entity);
    const auto& parentSize = registry.get<Sizable>(entity).size;
    const auto& hierarchy = registry.get<Hierarchy>(entity);

    std::vector<StackChild> children;
    children.reserve(hierarchy.children.size());
    for (auto child : hierarchy.children) {
        auto* element = registry.try_get<LayoutElement>(child);
        auto* sizable = registry.try_get<Sizable>(child);
        auto* transform = registry.try_get<LocalTransform>(child);
        if (!element || !sizable || !transform || element->ignoreLayout) {
            continue;
        }
        children.push_back({child, element, sizable, transform});
    }

    if (children.empty()) {
        return;
    }

    const bool horizontal = layout.direction == StackLayout::Direction::Horizontal;
    const int mainAxis = horizontal ? 0 : 1;
    const int crossAxis = horizontal ? 1 : 0;

    const float parentMain = horizontal ? parentSize.x : parentSize.y;
    const float parentCross = horizontal ? parentSize.y : parentSize.x;
    const float paddingMainStart = horizontal ? layout.paddingMin.x : layout.paddingMin.y;
    const float paddingMainEnd = horizontal ? layout.paddingMax.x : layout.paddingMax.y;
    const float paddingCrossStart = horizontal ? layout.paddingMin.y : layout.paddingMin.x;
    const float paddingCrossEnd = horizontal ? layout.paddingMax.y : layout.paddingMax.x;
    const float availableMain = std::max(0.0f, parentMain - paddingMainStart - paddingMainEnd - layout.spacing * std::max(0, (int)children.size() - 1));
    const float availableCross = std::max(0.0f, parentCross - paddingCrossStart - paddingCrossEnd);

    float totalPreferred = 0.0f;
    float totalMin = 0.0f;
    float totalGrow = 0.0f;
    float totalShrink = 0.0f;

    std::vector<vec2> sizes(children.size());
    for (size_t i = 0; i < children.size(); ++i) {
        auto& element = *children[i].element;
        totalPreferred += element.preferred[mainAxis];
        totalMin += element.min[mainAxis];
        totalGrow += element.grow[mainAxis];
        totalShrink += element.shrink[mainAxis];
        sizes[i] = element.preferred;
    }

    if (availableMain < totalPreferred && totalPreferred > totalMin && totalShrink > 0.0f) {
        const float deficit = totalPreferred - availableMain;
        for (size_t i = 0; i < children.size(); ++i) {
            auto& element = *children[i].element;
            const float share = deficit * (element.shrink[mainAxis] / totalShrink);
            sizes[i][mainAxis] = std::max(element.min[mainAxis], element.preferred[mainAxis] - share);
        }
    } else if (availableMain > totalPreferred && totalGrow > 0.0f) {
        const float extra = availableMain - totalPreferred;
        for (size_t i = 0; i < children.size(); ++i) {
            auto& element = *children[i].element;
            const float share = extra * (element.grow[mainAxis] / totalGrow);
            sizes[i][mainAxis] = std::min(element.max[mainAxis], element.preferred[mainAxis] + share);
        }
    } else {
        for (size_t i = 0; i < children.size(); ++i) {
            auto& element = *children[i].element;
            sizes[i][mainAxis] = ResolveAlongAxis(availableMain, element.min[mainAxis], element.preferred[mainAxis], element.max[mainAxis]);
        }
    }

    float cursor = paddingMainStart;
    for (size_t i = 0; i < children.size(); ++i) {
        auto& element = *children[i].element;
        auto& childSize = sizes[i];

        if (layout.crossAlign == StackLayout::Align::Stretch) {
            childSize[crossAxis] = availableCross;
        } else {
            childSize[crossAxis] = ResolveAlongAxis(availableCross, element.min[crossAxis], element.preferred[crossAxis], element.max[crossAxis]);
        }

        vec3 position = children[i].transform->position;
        if (horizontal) {
            position.x = cursor;
            switch (layout.crossAlign) {
                case StackLayout::Align::Start: position.y = paddingCrossStart; break;
                case StackLayout::Align::Center: position.y = paddingCrossStart + (availableCross - childSize.y) * 0.5f; break;
                case StackLayout::Align::End: position.y = parentSize.y - paddingCrossEnd - childSize.y; break;
                case StackLayout::Align::Stretch: position.y = paddingCrossStart; break;
            }
        } else {
            position.y = cursor;
            switch (layout.crossAlign) {
                case StackLayout::Align::Start: position.x = paddingCrossStart; break;
                case StackLayout::Align::Center: position.x = paddingCrossStart + (availableCross - childSize.x) * 0.5f; break;
                case StackLayout::Align::End: position.x = parentSize.x - paddingCrossEnd - childSize.x; break;
                case StackLayout::Align::Stretch: position.x = paddingCrossStart; break;
            }
        }

        if (children[i].sizable->size != childSize) {
            children[i].sizable->size = childSize;
            registry.patch<Sizable>(children[i].entity);
        }

        if (children[i].transform->position != position) {
            children[i].transform->position = position;
            registry.patch<LocalTransform>(children[i].entity);
        }

        cursor += childSize[mainAxis] + layout.spacing;

        if (registry.all_of<StackLayout, Sizable, Hierarchy>(children[i].entity)) {
            ApplyLayout(children[i].entity);
        }
    }
}
