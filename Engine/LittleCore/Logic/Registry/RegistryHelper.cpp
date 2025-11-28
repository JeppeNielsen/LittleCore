//
// Created by Jeppe Nielsen on 24/10/2025.
//

#include "RegistryHelper.hpp"
#include <vector>
#include "Hierarchy.hpp"

using namespace LittleCore;

void FindAllChildren(entt::registry& registry, entt::entity entity, std::vector<entt::entity>& children) {
    children.push_back(entity);

    Hierarchy& hierarchy = registry.get<Hierarchy>(entity);
    for(auto child : hierarchy.children) {
        FindAllChildren(registry, child, children);
    }
}

void CloneComponents(entt::registry &srcReg,
                           entt::entity src,
                           entt::registry &dstReg,
                           entt::entity dst,
                           const std::unordered_map<entt::entity, entt::entity>& originalToDuplicate) {
    for (auto [id, srcStorage] : srcReg.storage()) {
        if (srcStorage.contains(src)) {
            auto dstStorage = dstReg.storage(id);
            if (dstStorage == nullptr) {
                srcStorage.AssureTypeInRegistry(dstReg);
                dstStorage = dstReg.storage(id);
            }

            dstStorage->push(dst, srcStorage.value(src));
            dstStorage->InvokeComponentCloned(srcReg, src, dstReg, dst, originalToDuplicate);
        }
    }
}

entt::entity RegistryHelper::Duplicate(entt::registry& registry, entt::entity source, entt::registry& destRegistry, const Callback& callback) {
    std::vector<entt::entity> entitiesToDuplicate;
    FindAllChildren(registry, source, entitiesToDuplicate);
    std::unordered_map<entt::entity, entt::entity> originalToDuplicate;

    for(auto entityToDuplicate : entitiesToDuplicate) {
        auto duplicate = destRegistry.create();
        originalToDuplicate[entityToDuplicate] = duplicate;
    }

    for(auto entityToDuplicate : entitiesToDuplicate) {
        CloneComponents(registry, entityToDuplicate, destRegistry, originalToDuplicate[entityToDuplicate], originalToDuplicate);
    }
    if (callback) {
        for (auto entityToDuplicate : entitiesToDuplicate) {
            callback(entityToDuplicate, originalToDuplicate[entityToDuplicate]);
        }
    }


    return originalToDuplicate[source];
}

void RegistryHelper::TraverseHierarchy(entt::registry& registry, entt::entity root,
                                      const std::function<void(entt::entity)>& onEntity) {
    if (!registry.any_of<Hierarchy>(root)) {
        return;
    }

    onEntity(root);

    auto& hierarchy = registry.get<Hierarchy>(root);
    for(auto child : hierarchy.children) {
        TraverseHierarchy(registry, child, onEntity);
    }
}

entt::entity RegistryHelper::FindParent(entt::registry& registry, entt::entity source,
                                        const std::function<bool(entt::entity)>& predicate) {
    while (true) {
        if (predicate(source)) {
            return source;
        }
        source = registry.get<Hierarchy>(source).parent;

        if (!registry.valid(source)) {
            return entt::null;
        }
    }
    return entt::null;
}

entt::entity RegistryHelper::GetDuplicated(const std::unordered_map<entt::entity, entt::entity>& originalToDuplicate, entt::entity entity) {
    auto it = originalToDuplicate.find(entity);
    return it==originalToDuplicate.end() ? entity : it->second;
}
