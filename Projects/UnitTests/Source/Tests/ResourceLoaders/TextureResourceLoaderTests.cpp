//
// Created by Jeppe Nielsen on 03/03/2024.
//
#include "gtest/gtest.h"
#include "ResourceManager.hpp"
#include "TextureResourceLoaderFactory.hpp"

using namespace LittleCore;

namespace {
    TEST(TextureResourceLoader, TestLoad) {

        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper pathMapper;
        pathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<TextureResourceLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<TextureResourceLoaderFactory>();

        ResourceHandle<TextureResource> textureHandle = resourceManager.Create<TextureResource>("B62D424BF40F46359248CDE498930422");

        auto ig =  textureHandle->handle.idx;



        EXPECT_NE(textureHandle->handle.idx, bgfx::kInvalidHandle);

    }
}

