//
// Created by Jeppe Nielsen on 03/03/2024.
//
#include "gtest/gtest.h"
#include "ResourceManager.hpp"
#include "TextureResourceLoaderFactory.hpp"

using namespace LittleCore;

namespace {
    TEST(TextureResourceLoader, TestLoad) {


        bgfx::Init init;
        init.type = bgfx::RendererType::Noop;
        init.resolution.reset = BGFX_RESET_VSYNC;
        init.platformData.ndt = nullptr;
        init.platformData.nwh = nullptr;
        init.platformData.context = nullptr;
        init.platformData.backBuffer = nullptr;
        init.platformData.backBufferDS = nullptr;

        bgfx::init(init);
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

