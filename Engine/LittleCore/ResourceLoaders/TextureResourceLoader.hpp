//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include "ResourceLoader.hpp"
#include "Texturable.hpp"

namespace LittleCore {
    class TextureResourceLoader : public IResourceLoader<Texturable> {

        void Load(Texturable& texturable) override;
        void Unload(Texturable& texturable) override;
        bool IsLoaded() override;
        void Reload(Texturable& texturable) override;

    };
}

