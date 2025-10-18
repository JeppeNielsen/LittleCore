//
// Created by Jeppe Nielsen on 26/12/2024.
//

#pragma once
#include "ResourceManager.hpp"
#include "ShaderResourceLoaderFactory.hpp"
#include "TextureResourceLoaderFactory.hpp"
#include "MeshResourceLoaderFactory.hpp"

namespace LittleCore {

    using DefaultResourceManager = ResourceManager<ShaderResourceLoaderFactory, TextureResourceLoaderFactory, MeshResourceLoaderFactory>;

}