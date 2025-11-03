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
#include "PrefabExposedComponents.hpp"

using namespace LittleCore;

namespace {

    using ResourcesManager = ResourceManager<PrefabResourceLoaderFactory>;

    struct PrefabSerializer : ComponentSerializerBase<Prefab, std::string> {

        ResourcesManager* resourceManager;

        void SetResourceManager(ResourcesManager& defaultResourceManager) {
            resourceManager = &defaultResourceManager;
        }

        void Serialize(const Prefab& prefab, std::string& id) {
            auto info = resourceManager->GetInfo(prefab.resource);
            id = info.id;
        }

        void Deserialize(const std::string& id, Prefab& prefab) {
            prefab.resource = resourceManager->Create<PrefabResource>(id);
        }
    };

    TEST(PrefabSystem, TestInstance) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);

        RegistrySerializer<LocalTransform, Hierarchy, PrefabSerializer> serializer;
        serializer.GetSerializer<PrefabSerializer>().SetResourceManager(resourceManager);

        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(serializer);

        entt::registry registry;
        HierarchySystem hierarchySystem(registry);
        PrefabSystem prefabSystem(registry);

        auto instanceEntity = registry.create();
        auto& instanceHierarchy = registry.emplace<Hierarchy>(instanceEntity);
        auto& local = registry.emplace<LocalTransform>(instanceEntity);
        auto& prefab = registry.emplace<Prefab>(instanceEntity);
        prefab.resource = resourceManager.Create<PrefabResource>("9953944CCE324C019F30699342FF9AE0");

        hierarchySystem.Update();
        prefabSystem.Update();

        EXPECT_TRUE(prefab.roots.size() == 1);
        auto localTransform = registry.get<LocalTransform>(prefab.roots[0]);
        EXPECT_EQ(localTransform.position.x,1.0f);
        EXPECT_EQ(localTransform.position.y,2.0f);
        EXPECT_EQ(localTransform.position.z,3.0f);
    }

    TEST(PrefabSystem, TestNestedPrefabs) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);

        RegistrySerializer<LocalTransform, Hierarchy, PrefabSerializer> serializer;
        serializer.GetSerializer<PrefabSerializer>().SetResourceManager(resourceManager);

        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(serializer);


        entt::registry registry;
        HierarchySystem hierarchySystem(registry);
        PrefabSystem prefabSystem(registry);

        auto instanceEntity = registry.create();
        auto& instanceHierarchy = registry.emplace<Hierarchy>(instanceEntity);
        auto& local = registry.emplace<LocalTransform>(instanceEntity);
        auto& prefab = registry.emplace<Prefab>(instanceEntity);
        prefab.resource = resourceManager.Create<PrefabResource>("DAEFD5275FF944E7BC65614E634E85D1");


        prefabSystem.Update();
        hierarchySystem.Update();

        EXPECT_EQ(prefab.roots.size(), 1);
        auto& hierarchy = registry.get<Hierarchy>(prefab.roots[0]);
        auto& prefabHandle = registry.get<Prefab>(prefab.roots[0]);
        auto& nestedHierarchy = registry.get<Hierarchy>(hierarchy.children[0]);

        EXPECT_EQ(hierarchy.children.size(), 1);
        EXPECT_EQ(nestedHierarchy.children.size(),0);
    }


    TEST(PrefabSystem, TestExposedComponents) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);

        RegistrySerializer<LocalTransform, Hierarchy, PrefabSerializer, PrefabExposedComponents> serializer;
        serializer.GetSerializer<PrefabSerializer>().SetResourceManager(resourceManager);

        resourceManager.CreateLoaderFactory<PrefabResourceLoaderFactory>(serializer);


        entt::registry registry;
        HierarchySystem hierarchySystem(registry);
        PrefabSystem prefabSystem(registry);
        prefabSystem.SetSerializer(serializer);

        auto instanceEntity = registry.create();
        auto& instanceHierarchy = registry.emplace<Hierarchy>(instanceEntity);
        auto& local = registry.emplace<LocalTransform>(instanceEntity);
        auto& prefab = registry.emplace<Prefab>(instanceEntity);
        prefab.resource = resourceManager.Create<PrefabResource>("BBE8C8BC37C24312BCC239D8651E8137");
        SerializedPrefabComponent serializedPrefabComponent;
        serializedPrefabComponent.sourceEntity = (entt::entity)0;
        serializedPrefabComponent.componentId = "LittleCore::LocalTransform";
        serializedPrefabComponent.data = "{\n"
                                         "                  \"position\": {\n"
                                         "                     \"x\": 55,\n"
                                         "                     \"y\": 66,\n"
                                         "                     \"z\": 77\n"
                                         "                  },\n"
                                         "                  \"rotation\": {\n"
                                         "                     \"x\": 0,\n"
                                         "                     \"y\": 0,\n"
                                         "                     \"z\": 0,\n"
                                         "                     \"w\": 1\n"
                                         "                  },\n"
                                         "                  \"scale\": {\n"
                                         "                     \"x\": 1,\n"
                                         "                     \"y\": 1,\n"
                                         "                     \"z\": 1\n"
                                         "                  }\n"
                                         "               }";

        prefab.components.push_back(serializedPrefabComponent);

        prefabSystem.Update();
        hierarchySystem.Update();


        auto& localTransform = registry.get<LocalTransform>(prefab.roots[0]);

        EXPECT_EQ(localTransform.position.x, 55);
        EXPECT_EQ(localTransform.position.y, 66);
        EXPECT_EQ(localTransform.position.z, 77);


    }












}