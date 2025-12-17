//
// Created by Jeppe Nielsen on 01/12/2025.
//

#include "FontResourceLoader.hpp"

using namespace LittleCore;

FontResourceLoader::FontResourceLoader() {

}

void FontResourceLoader::Load(FontResource& resource) {
    resource.atlasDynamic.initFromFile(path.c_str(), 64, {});
}

void FontResourceLoader::Unload(FontResource& resource) {
    resource.atlasDynamic.destroy();
}

bool FontResourceLoader::IsLoaded() {
    return true;
}

void FontResourceLoader::Reload(FontResource& resource) {
    Unload(resource);
    Load(resource);
}
