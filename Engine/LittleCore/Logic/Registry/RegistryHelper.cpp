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
        }
    }

    return dst;
}

entt::entity clone_entity(entt::registry& registry, entt::entity src) {
    entt::entity dst = registry.create();

    for(auto [id, storage]: registry.storage()) {
        if(storage.contains(src)) {



            storage.push(dst, storage.value(src));
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

        duplicateHierarchy.children.clear();
        duplicateHierarchy.previousParent = entt::null;

        duplicateHierarchy.parent = originalHierarchy.parent == entt::null ? entt::null : originalToDuplicate[originalHierarchy.parent];
    }

    return originalToDuplicate[source];
}
