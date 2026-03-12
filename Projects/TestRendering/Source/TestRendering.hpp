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
#include "Texturable.hpp"

using namespace LittleCore;

using Resources = ResourceManager<TextureResourceLoaderFactory, ShaderResourceLoaderFactory>;

class TestRendering : public IState {
    ResourcePathMapper resourcePathMapper;
    CustomSimulation<MovableSystem> simulation;
    SokolRenderer renderer;
    Resources resources;
    entt::entity cameraEntity;
    ResourceHandle<ShaderResource> shader;
    ResourceHandle<Texturable> texture;
    ResourceHandle<Texturable> texture2;
    float time;
    ImGuiController imGuiController;
    SDLInputHandler inputHandler;

    const uint16_t renderTextureWidth = 1024;
    const uint16_t renderTextureHeight = 1024;
    sg_image renderTexture = {SG_INVALID_ID};
    sg_image depthTexture = {SG_INVALID_ID};
    sg_attachments framebuffer = {SG_INVALID_ID};
    entt::entity quad1;
    entt::entity quad2;
    entt::entity cameraObject;

public:
    TestRendering();
    ~TestRendering() override;

    void Initialize() override;

    void Update(float dt) override;

    void Render() override;

    void HandleEvent(void* event) override;

};
