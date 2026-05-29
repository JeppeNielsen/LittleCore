//
// Created by Codex on 03/05/2026.
//

#include "SpriteMeshSystem.hpp"
#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>
#include "Mesh.hpp"
#include "Sizable.hpp"
#include "Sprite.hpp"

using namespace LittleCore;

namespace {

    constexpr float SliceEpsilon = 1e-6f;
    constexpr uint32_t VertexColor = 0xFFFFFFFF;

    struct SlicePoint {
        float position;
        float uv;
    };

    bool IsClose(const float a, const float b) {
        return std::abs(a - b) <= SliceEpsilon;
    }

    bool IsSingleQuad(const Sprite& sprite) {
        return IsClose(sprite.slicingLeft, 0.0f) &&
               IsClose(sprite.slicingRight, 0.0f) &&
               IsClose(sprite.slicingTop, 0.0f) &&
               IsClose(sprite.slicingBottom, 0.0f);
    }

    std::pair<float, float> ResolveSliceCuts(float startPercent, float endPercent) {
        startPercent = std::clamp(startPercent, 0.0f, 1.0f);
        endPercent = std::clamp(endPercent, 0.0f, 1.0f);

        const auto sum = startPercent + endPercent;
        if (sum > 1.0f && sum > SliceEpsilon) {
            const auto scale = 1.0f / sum;
            startPercent *= scale;
            endPercent *= scale;
        }

        return {startPercent, 1.0f - endPercent};
    }

    std::vector<SlicePoint> BuildAxisSlicePoints(float startPercent, float endPercent, float targetSize, float cornerExtent) {
        targetSize = std::max(targetSize, 0.0f);
        cornerExtent = std::max(cornerExtent, 0.0f);

        const auto [resolvedStartUv, resolvedEndUv] = ResolveSliceCuts(startPercent, endPercent);
        const bool isSlicedAxis =
                cornerExtent > SliceEpsilon &&
                (resolvedEndUv - resolvedStartUv) > SliceEpsilon &&
                (resolvedStartUv > SliceEpsilon || resolvedEndUv < (1.0f - SliceEpsilon));

        if (!isSlicedAxis) {
            return {
                    {0.0f, 0.0f},
                    {targetSize, 1.0f}
            };
        }

        cornerExtent = std::min(cornerExtent, targetSize * 0.5f);

        return {
                {0.0f, 0.0f},
                {cornerExtent, resolvedStartUv},
                {std::max(targetSize - cornerExtent, 0.0f), resolvedEndUv},
                {targetSize, 1.0f}
        };
    }

    void BuildSingleQuadMesh(Mesh& mesh, const Sizable& sizable) {
        mesh.triangles = {0, 1, 2, 0, 2, 3};
        mesh.vertices = {
                {{0.0f, 0.0f, 0.0f}, VertexColor, {0.0f, 0.0f}},
                {{sizable.size.x, 0.0f, 0.0f}, VertexColor, {1.0f, 0.0f}},
                {{sizable.size.x, sizable.size.y, 0.0f}, VertexColor, {1.0f, 1.0f}},
                {{0.0f, sizable.size.y, 0.0f}, VertexColor, {0.0f, 1.0f}}
        };
    }

    void BuildSlicedMesh(Mesh& mesh, const Sizable& sizable, const Sprite& sprite) {
        const auto xSlicePoints = BuildAxisSlicePoints(
                sprite.slicingLeft,
                sprite.slicingRight,
                sizable.size.x,
                sprite.cornerSize.x);
        const auto ySlicePoints = BuildAxisSlicePoints(
                sprite.slicingBottom,
                sprite.slicingTop,
                sizable.size.y,
                sprite.cornerSize.y);

        mesh.vertices.clear();
        mesh.triangles.clear();

        mesh.vertices.reserve(xSlicePoints.size() * ySlicePoints.size());
        mesh.triangles.reserve((xSlicePoints.size() - 1) * (ySlicePoints.size() - 1) * 6);

        for (const auto& ySlicePoint : ySlicePoints) {
            for (const auto& xSlicePoint : xSlicePoints) {
                mesh.vertices.push_back({
                        {xSlicePoint.position, ySlicePoint.position, 0.0f},
                        VertexColor,
                        {xSlicePoint.uv, ySlicePoint.uv}
                });
            }
        }

        const auto columns = static_cast<uint16_t>(xSlicePoints.size());
        const auto rows = static_cast<uint16_t>(ySlicePoints.size());

        for (uint16_t y = 0; y + 1 < rows; ++y) {
            for (uint16_t x = 0; x + 1 < columns; ++x) {
                const auto bottomLeft = static_cast<uint16_t>(y * columns + x);
                const auto bottomRight = static_cast<uint16_t>(bottomLeft + 1);
                const auto topLeft = static_cast<uint16_t>(bottomLeft + columns);
                const auto topRight = static_cast<uint16_t>(topLeft + 1);

                mesh.triangles.push_back(bottomLeft);
                mesh.triangles.push_back(bottomRight);
                mesh.triangles.push_back(topRight);
                mesh.triangles.push_back(bottomLeft);
                mesh.triangles.push_back(topRight);
                mesh.triangles.push_back(topLeft);
            }
        }
    }

}

SpriteMeshSystem::SpriteMeshSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector
                .update<Sizable>().where<Mesh, Sprite>()
                .update<Sprite>().where<Sizable, Mesh>()
                .group<Sizable, Sprite, Mesh>()) {
    registry.on_construct<Sizable>().connect<&SpriteMeshSystem::SizableAdded>(this);
    registry.on_construct<Mesh>().connect<&SpriteMeshSystem::MeshAdded>(this);
    registry.on_construct<Sprite>().connect<&SpriteMeshSystem::SpriteAdded>(this);
}

void SpriteMeshSystem::Update() {
    for (auto entity : observer) {
        auto& mesh = registry.get<Mesh>(entity);
        const auto& sizeable = registry.get<Sizable>(entity);
        const auto& sprite = registry.get<Sprite>(entity);

        if (mesh.handle) {
            mesh.handle.Clear();
        }

        if (IsSingleQuad(sprite)) {
            BuildSingleQuadMesh(mesh, sizeable);
        } else {
            BuildSlicedMesh(mesh, sizeable, sprite);
        }

        registry.patch<Mesh>(entity);
    }

    observer.clear();
}

void SpriteMeshSystem::SizableAdded(entt::registry& registry, entt::entity entity) {
    if (!registry.all_of<Mesh, Sprite>(entity)) {
        return;
    }

    registry.patch<Sizable>(entity);
}

void SpriteMeshSystem::MeshAdded(entt::registry& registry, entt::entity entity) {
    if (!registry.all_of<Sizable, Sprite>(entity)) {
        return;
    }

    registry.patch<Sizable>(entity);
}

void SpriteMeshSystem::SpriteAdded(entt::registry& registry, entt::entity entity) {
    if (!registry.all_of<Sizable, Mesh>(entity)) {
        return;
    }

    registry.patch<Sprite>(entity);
}
