//
// Created by Jeppe Nielsen on 03/03/2024.
//

#pragma once
#include <vector>
#include "ResourceLoader.hpp"
#include "FontResource.hpp"
#include <msdfgen-ext.h>

namespace LittleCore {
    class FontResourceLoader : public IResourceLoader<FontResource> {
    public:
        FontResourceLoader(msdfgen::FreetypeHandle* freetypeHandle);
        void Load(FontResource& resource) override;
        void Unload(FontResource& resource) override;
        bool IsLoaded() override;
        void Reload(FontResource& resource) override;
    private:
        msdfgen::FreetypeHandle* freetypeHandle;
    };
}

