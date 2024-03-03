//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include "ResourceLoader.hpp"
#include "Texture.hpp"

namespace LittleCore {
    class TextureLoader : public IResourceLoader<Texture> {

        void Load(Texture& resource) override {

        }

        void Unload(Texture& resource) override {

        }

        bool IsLoaded() override {
            return false;
        }

    };
}

