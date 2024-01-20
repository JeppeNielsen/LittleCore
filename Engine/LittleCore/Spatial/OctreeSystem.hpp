//
//  QuadTreeSystem.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <entt/entt.hpp>
#include "WorldBoundingBox.hpp"
#include "Octree.hpp"
#include <unordered_map>

namespace LittleCore {
    template<typename ...T>
    struct OctreeSystem {
        
        OctreeSystem(entt::registry& registry) : registry(registry) {
            octree.SetBoundingBox({{0,0,0}, {100000,100000,100000}});
            defaultNode.node = nullptr;
            observer.connect(registry, entt::collector.group<WorldBoundingBox, T...>());
        }

        void Update() {
            for(auto entity : observer) {
                UpdateEntity(entity);
            }
        }
    
        void UpdateEntity(entt::entity entity) {

            auto& worldBoundingBox = registry.get<WorldBoundingBox>(entity);

            auto& node = nodes[entity];
            node.data = entity;
            node.box = worldBoundingBox.bounds;
            
            if (!node.node) {
                octree.Insert(node);
            } else {
                octree.Move(node);
            }
        }
        
        /*void GameObjectRemoved(Tiny::GameObject gameObject) {
            if (gameObject >= nodes.size()) {
                return;
            }
            
            auto& node = nodes[gameObject];
            octree.Remove(node);
        }
         */
        
        void Query(const BoundingFrustum& frustum, std::vector<entt::entity>& entities) const {
            octree.Get(frustum, entities);
        }
        
        void Query(const Ray& ray, std::vector<entt::entity>& entities) const {
            octree.Get(ray, entities);
        }
        
        void Query(const BoundingBox& box, std::vector<entt::entity>& entities) const {
            octree.Get(box, entities);
        }

        entt::registry& registry;
        entt::observer observer;

        Octree<entt::entity> octree;
        std::unordered_map<entt::entity, OctreeNode<entt::entity>> nodes;
        OctreeNode<entt::entity> defaultNode;

   };
}
