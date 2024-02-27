//
// Created by Jeppe Nielsen on 27/02/2024.
//
#include "gtest/gtest.h"
#include "ResourceManager.hpp"

using namespace LittleCore;

namespace {

    struct Mesh {
        int vertices;
    };

    struct MeshLoader : IResourceLoader<Mesh> {

        void Load(Mesh& resource) override {

        }
        void Unload(Mesh& resource) override {

        }
        virtual bool IsLoaded() override {
            return true;
        }
    };

    struct MeshLoaderFactory : IResourceLoaderFactory<MeshLoader> {
        Loader Create() {
            return CreateLoader();
        }
    };

    TEST(ResourceManagerTest, EmptyHandle) {
        ResourceHandle<int> handle;
        ASSERT_FALSE(handle);
    }

    TEST(ResourceManagerTest, ValidHandle) {

        ResourceManager<MeshLoaderFactory> resourceManager;
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle = resourceManager.Create<Mesh>("Cube.obj");

        ASSERT_TRUE(handle);
    }

    TEST(ResourceManagerTest, HandleCopyCtor) {

        ResourceManager<MeshLoaderFactory> resourceManager;
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
        auto handle2= handle1;

        ASSERT_EQ(handle1.operator->(), handle2.operator->());
    }

    TEST(ResourceManagerTest, HandlePointerSame) {

        ResourceManager<MeshLoaderFactory> resourceManager;
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
        auto handle2 = resourceManager.Create<Mesh>("Cube.obj");

        ASSERT_EQ(handle1.operator->(), handle2.operator->());
    }


}