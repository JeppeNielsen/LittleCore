//
// Created by Jeppe Nielsen on 03/03/2024.
//
#include "gtest/gtest.h"
#include "ResourceManager.hpp"
#include "TextureResourceLoaderFactory.hpp"
#include "SokolDirect.hpp"

using namespace LittleCore;

namespace {
    TEST(TextureResourceLoader, TestLoad) {


        ASSERT_TRUE(lc_sg_setup());
        std::string rootPath = "../../../../Assets/";

        ResourcePathMapper pathMapper;
        pathMapper.RefreshFromRootPath(rootPath);
        ResourceManager<TextureResourceLoaderFactory> resourceManager(pathMapper);
        resourceManager.CreateLoaderFactory<TextureResourceLoaderFactory>();

        ResourceHandle<Texturable> textureHandle = resourceManager.Create<Texturable>("B62D424BF40F46359248CDE498930422");

        EXPECT_NE(textureHandle->texture.id, SG_INVALID_ID);

        lc_sg_shutdown();
    }
}
