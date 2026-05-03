//
// Created by Codex on 03/05/2026.
//

#include "SizableSystem.hpp"
#include "Mesh.hpp"
#include "Sizable.hpp"

using namespace LittleCore;

SizableSystem::SizableSystem(entt::registry& registry) :
        SystemBase(registry),
        observer(registry, entt::collector.update<Sizable>().group<Mesh, Sizable>()) {
    registry.on_construct<Sizable>().connect<&SizableSystem::SizableAdded>(this);
    registry.on_construct<Mesh>().connect<&SizableSystem::MeshAdded>(this);
}

void SizableSystem::Update() {
    for (auto entity : observer) {
        auto& mesh = registry.get<Mesh>(entity);
        const auto& sizeable = registry.get<Sizable>(entity);

        if (mesh.handle) {
            mesh.handle.Clear();
        }

        if (mesh.triangles.size() != 6) {
            mesh.triangles = {0, 1, 2, 0, 2, 3};
        }

        mesh.vertices = {
                {{0, 0, 0}, 0xFFFFFFFF, {0, 0}},
                {{sizeable.size.x, 0, 0}, 0xFFFFFFFF, {1, 0}},
                {{sizeable.size.x, sizeable.size.y, 0}, 0xFFFFFFFF, {1, 1}},
                {{0, sizeable.size.y, 0}, 0xFFFFFFFF, {0, 1}}
        };

        registry.patch<Mesh>(entity);
    }

    observer.clear();
}

void SizableSystem::SizableAdded(entt::registry& registry, entt::entity entity) {
    if (!registry.all_of<Mesh>(entity)) {
        return;
    }

    registry.patch<Sizable>(entity);
}

void SizableSystem::MeshAdded(entt::registry& registry, entt::entity entity) {
    if (!registry.all_of<Sizable>(entity)) {
        return;
    }

    registry.patch<Sizable>(entity);
}
