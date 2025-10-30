//
// Created by Jeppe Nielsen on 24/10/2025.
//

#include "RegistryHelper.hpp"
#include <vector>
#include "Hierarchy.hpp"
#include "IgnoreSerialization.hpp"
using namespace LittleCore;


void FindAllChildren(entt::registry& registry, entt::entity entity, std::vector<entt::entity>& children) {

    if (registry.any_of<IgnoreSerialization>(entity)) {
        return;
    }

    children.push_back(entity);

    Hierarchy& hierarchy = registry.get<Hierarchy>(entity);
    for(auto child : hierarchy.children) {
        FindAllChildren(registry, child, children);
    }
}

entt::entity clone_between(entt::registry &srcReg,
                           entt::entity   src,
                           entt::registry &dstReg) {
    const entt::entity dst = dstReg.create();

    // Iterate all storages known to the *source* registry
    for (auto [id, srcStorage] : srcReg.storage()) {
        if (srcStorage.contains(src)) {
            // Get/create the matching storage in the *destination* registry

            auto dstStorage = dstReg.storage(id);
            if (dstStorage == nullptr) {
                srcStorage.AssureTypeInRegistry(dstReg);
                dstStorage = dstReg.storage(id);
            }

            // Copy the component value over
            dstStorage->push(dst, srcStorage.value(src));
            //dstStorage->PatchEntity(dstReg, dst);
        }
    }

    return dst;
}

entt::entity RegistryHelper::Duplicate(entt::registry& registry, entt::entity source, entt::registry& destRegistry) {
    std::vector<entt::entity> entitiesToDuplicate;
    FindAllChildren(registry, source, entitiesToDuplicate);
    std::unordered_map<entt::entity, entt::entity> originalToDuplicate;

    for(auto entityToDuplicate : entitiesToDuplicate) {
        auto duplicate = clone_between(registry, entityToDuplicate, destRegistry);
        originalToDuplicate[entityToDuplicate] = duplicate;
    }

    for(auto[orignal, duplicate] : originalToDuplicate) {
        Hierarchy& originalHierarchy = registry.get<Hierarchy>(orignal);
        Hierarchy& duplicateHierarchy = destRegistry.get<Hierarchy>(duplicate);

        if (originalHierarchy.parent == entt::null) {
            duplicateHierarchy.parent = entt::null;
        } else {
            duplicateHierarchy.parent = originalToDuplicate[originalHierarchy.parent];
            destRegistry.get<Hierarchy>(duplicateHierarchy.parent).children.push_back(duplicate);
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
