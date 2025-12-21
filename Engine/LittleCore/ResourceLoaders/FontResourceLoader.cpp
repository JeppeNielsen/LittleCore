//
// Created by Jeppe Nielsen on 01/12/2025.
//

#include "FontResourceLoader.hpp"

using namespace LittleCore;

FontResourceLoader::FontResourceLoader() {

}

void FontResourceLoader::Load(FontResource& resource) {
    resource.atlas.initFromFile(path.c_str(), 64, {});
}

void FontResourceLoader::Unload(FontResource& resource) {
    resource.atlas.destroy();
}

bool FontResourceLoader::IsLoaded() {
    return true;
}

void FontResourceLoader::Reload(FontResource& resource) {
    Unload(resource);
    Load(resource);
}
