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

        void Load(Mesh& resource) override {}

        void Unload(Mesh& resource) override {}

        bool IsLoaded() override {
            return true;
        }

        void Reload(Mesh& resource) override {

        }
    };

    struct MeshLoaderFactory : IResourceLoaderFactory<MeshLoader> {
        Loader Create() override {
            return CreateLoader();
        }
    };

    TEST(ResourceManagerTest, EmptyHandle) {
        ResourceHandle<int> handle;
        EXPECT_FALSE(handle);
    }

    TEST(ResourceManagerTest, ValidHandle) {

        ResourcePathMapper pathMapper;
        ResourceManager<MeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle = resourceManager.Create<Mesh>("Cube.obj");

        EXPECT_TRUE(handle);
    }

    TEST(ResourceManagerTest, HandleCopyCtor) {
        ResourcePathMapper pathMapper;
        ResourceManager<MeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
        auto handle2= handle1;

        EXPECT_EQ(handle1.operator->(), handle2.operator->());
    }

    TEST(ResourceManagerTest, HandlePointerSame) {

        ResourcePathMapper pathMapper;
        ResourceManager<MeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
        auto handle2 = resourceManager.Create<Mesh>("Cube.obj");

        EXPECT_EQ(handle1.operator->(), handle2.operator->());
    }

    TEST(ResourceManagerTest, HandlePointerDifferent) {

        ResourcePathMapper pathMapper;
        ResourceManager<MeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<MeshLoaderFactory>();

        auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
        auto handle2 = resourceManager.Create<Mesh>("Cube2.obj");

        EXPECT_NE(handle1.operator->(), handle2.operator->());
    }

    TEST(ResourceManagerTest, NoMoreReferencesCallUnload) {

        struct DebugMeshLoader : IResourceLoader<Mesh> {

            int* loadCounter;

            explicit DebugMeshLoader(int *loadCounter) : loadCounter(loadCounter) {}

            void Load(Mesh& resource) override {
                (*loadCounter)++;
            }
            void Unload(Mesh& resource) override {
                (*loadCounter)--;
            }
            bool IsLoaded() override {
                return true;
            }
            void Reload(Mesh& resource) override {

            }
        };

        struct DebugMeshLoaderFactory : IResourceLoaderFactory<DebugMeshLoader> {
            int* loadCounter;

            explicit DebugMeshLoaderFactory(int *loadCounter) : loadCounter(loadCounter) {}

            Loader Create() override {
                return CreateLoader(loadCounter);
            }
        };

        int loadCounter = 0;

        ResourcePathMapper pathMapper;
        ResourceManager<DebugMeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<DebugMeshLoaderFactory>(&loadCounter);

        {
            ResourceHandle<Mesh> topHandle;
            {
                auto handle1 = resourceManager.Create<Mesh>("Cube.obj");
                EXPECT_EQ(loadCounter, 1);

                topHandle = handle1;
            }

            EXPECT_EQ(loadCounter, 1);
        }

        EXPECT_EQ(loadCounter, 0);
    }

    TEST(ResourceManagerTest, NonLoadedResourceShouldReturnInvalidHandle) {

        struct NotLoadedMeshLoader : IResourceLoader<Mesh> {
            void Load(Mesh& resource) override {}
            void Unload(Mesh& resource) override {}
            bool IsLoaded() override { return false;}
            void Reload(Mesh& mesh) override {}
        };

        struct NotLoadedMeshLoaderFactory : IResourceLoaderFactory<NotLoadedMeshLoader> {
            Loader Create() override {
                return CreateLoader();
            }
        };

        ResourcePathMapper pathMapper;
        ResourceManager<NotLoadedMeshLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<NotLoadedMeshLoaderFactory>();

        auto handle = resourceManager.Create<Mesh>("Cube.obj");

        EXPECT_FALSE(handle);
    }

}