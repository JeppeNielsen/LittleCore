//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include "ResourceLoader.hpp"
#include "TextureResource.hpp"

namespace LittleCore {
    class TextureResourceLoader : public IResourceLoader<TextureResource> {

        void Load(TextureResource& resource) override;
        void Unload(TextureResource& resource) override;
        bool IsLoaded() override;
        void Reload(TextureResource& resource) override;

    };
}

