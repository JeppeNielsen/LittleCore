//
// Created by Jeppe Nielsen on 08/08/2024.
//


#pragma once
#include "IState.hpp"
#include <entt/entt.hpp>
#include "Simulation.hpp"
#include "ResourcePathMapper.hpp"
#include "ResourceManager.hpp"
#include "TextureResourceLoaderFactory.hpp"
#include "ShaderResourceLoaderFactory.hpp"

using namespace LittleCore;

using Resources = ResourceManager<TextureResourceLoaderFactory, ShaderResourceLoaderFactory>;

class TestRendering : public IState {
    ResourcePathMapper resourcePathMapper;
    entt::registry registry;
    Simulation simulation;
    BGFXRenderer bgfxRenderer;
    Resources resources;
    entt::entity cameraEntity;
    ResourceHandle<ShaderResource> shader;
    ResourceHandle<TextureResource> texture;
    bgfx::UniformHandle colorTexture;
    float time;
public:
    TestRendering();

    void Initialize() override;

    void Update(float dt) override;

    void Render() override;

};
