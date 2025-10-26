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

    TEST(PrefabSystem, TestInstance) {

        std::string rootPath = "../../../../Assets/";

        RegistrySerializer<LocalTransform, Hierarchy> serializer;

        ResourcePathMapper resourcePathMapper;
        resourcePathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<PrefabResourceLoaderFactory> resourceManager(resourcePathMapper);
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

        EXPECT_TRUE(prefabInstance.root!=entt::null);
        auto localTransform = registry.get<LocalTransform>(prefabInstance.root);
        EXPECT_EQ(localTransform.position.x,1.0f);
        EXPECT_EQ(localTransform.position.y,2.0f);
        EXPECT_EQ(localTransform.position.z,3.0f);
    }















}