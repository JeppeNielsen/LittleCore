//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "gtest/gtest.h"
#include "HierarchySystem.hpp"
#include "PrefabSystem.hpp"
#include "ResourceManager.hpp"
#include "PrefabResourceLoaderFactory.hpp"
#include "RegistrySerializer.hpp"
#include "LocalTransform.hpp"
#include "MathReflection.hpp"

using namespace LittleCore;

namespace {

    using ResourcesManager = ResourceManager<PrefabResourceLoaderFactory>;

    struct PrefabInstanceSerializer : ComponentSerializerBase<PrefabInstance, std::string> {

        ResourcesManager* resourceManager;

        void SetResourceManager(ResourcesManager& defaultResourceManager) {
            resourceManager = &defaultResourceManager;
        }

        void Serialize(const PrefabInstance& prefabInstance, std::string& id) {
            auto info = resourceManager->GetInfo(prefabInstance.Prefab);
            id = info.id;
        }

        void Deserialize(const std::string& id, PrefabInstance& prefabInstance) {
            prefabInstance.Prefab = resourceManager->Create<PrefabResource>(id);
        }
    };

    TEST(PrefabSystem, TestInstance) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);

        RegistrySerializer<LocalTransform, Hierarchy, PrefabInstanceSerializer> serializer;
        serializer.GetSerializer<PrefabInstanceSerializer>().SetResourceManager(resourceManager);

        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(serializer);

        entt::registry registry;
        HierarchySystem hierarchySystem(registry);
        PrefabSystem prefabSystem(registry);

        auto instanceEntity = registry.create();
        auto& instanceHierarchy = registry.emplace<Hierarchy>(instanceEntity);
        auto& local = registry.emplace<LocalTransform>(instanceEntity);
        auto& prefabInstance = registry.emplace<PrefabInstance>(instanceEntity);
        prefabInstance.Prefab = resourceManager.Create<PrefabResource>("9953944CCE324C019F30699342FF9AE0");

        hierarchySystem.Update();
        prefabSystem.Update();

        EXPECT_TRUE(prefabInstance.roots.size() == 1);
        auto localTransform = registry.get<LocalTransform>(prefabInstance.roots[0]);
        EXPECT_EQ(localTransform.position.x,1.0f);
        EXPECT_EQ(localTransform.position.y,2.0f);
        EXPECT_EQ(localTransform.position.z,3.0f);
    }

    TEST(PrefabSystem, TestNestedPrefabs) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);

        RegistrySerializer<LocalTransform, Hierarchy, PrefabInstanceSerializer> serializer;
        serializer.GetSerializer<PrefabInstanceSerializer>().SetResourceManager(resourceManager);

        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(serializer);


        entt::registry registry;
        HierarchySystem hierarchySystem(registry);
        PrefabSystem prefabSystem(registry);

        auto instanceEntity = registry.create();
        auto& instanceHierarchy = registry.emplace<Hierarchy>(instanceEntity);
        auto& local = registry.emplace<LocalTransform>(instanceEntity);
        auto& prefabInstance = registry.emplace<PrefabInstance>(instanceEntity);
        prefabInstance.Prefab = resourceManager.Create<PrefabResource>("DAEFD5275FF944E7BC65614E634E85D1");


        prefabSystem.Update();
        hierarchySystem.Update();

        EXPECT_EQ(prefabInstance.roots.size(), 1);
        auto& hierarchy = registry.get<Hierarchy>(prefabInstance.roots[0]);
        auto& prefabHandle = registry.get<PrefabInstance>(prefabInstance.roots[0]);
        auto& nestedHierarchy = registry.get<Hierarchy>(hierarchy.children[0]);

        EXPECT_EQ(hierarchy.children.size(), 1);
        EXPECT_EQ(nestedHierarchy.children.size(),0);
    }














}