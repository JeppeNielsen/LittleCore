//
// Created by Jeppe Nielsen on 08/08/2024.
//


#pragma once
#include "IState.hpp"
#include <entt/entt.hpp>
#include "DefaultSimulation.hpp"
#include "ResourcePathMapper.hpp"
#include "ResourceManager.hpp"
#include "TextureResourceLoaderFactory.hpp"
#include "ShaderResourceLoaderFactory.hpp"
#include "ImGuiController.hpp"
#include "MovableSystem.hpp"
#include "SDLInputHandler.hpp"

using namespace LittleCore;

using Resources = ResourceManager<TextureResourceLoaderFactory, ShaderResourceLoaderFactory>;

class TestRendering : public IState {
    ResourcePathMapper resourcePathMapper;
    entt::registry registry;
    CustomSimulation<MovableSystem> simulation;
    BGFXRenderer bgfxRenderer;
    Resources resources;
    entt::entity cameraEntity;
    ResourceHandle<ShaderResource> shader;
    ResourceHandle<TextureResource> texture;
    bgfx::UniformHandle colorTexture;
    float time;
    ImGuiController imGuiController;
    SDLInputHandler inputHandler;

    const uint16_t renderTextureWidth = 1024;
    const uint16_t renderTextureHeight = 512;
    bgfx::TextureHandle renderTexture;
    bgfx::FrameBufferHandle framebuffer;
    entt::entity quad1;
    entt::entity quad2;

public:
    TestRendering();

    void Initialize() override;

    void Update(float dt) override;

    void Render() override;

    void HandleEvent(void* event) override;

};
