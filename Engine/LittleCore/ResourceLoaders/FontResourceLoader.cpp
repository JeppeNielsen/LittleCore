//
// Created by Jeppe Nielsen on 01/12/2025.
//

#include "FontResourceLoader.hpp"

using namespace LittleCore;
using namespace msdfgen;

FontResourceLoader::FontResourceLoader(msdfgen::FreetypeHandle* freetypeHandle) :freetypeHandle(freetypeHandle) {

}

void FontResourceLoader::Load(FontResource& resource) {
    resource.font = loadFont(freetypeHandle, path.c_str());
    resource.atlasDynamic.initFromFile(path.c_str(), 64, {});
}

void FontResourceLoader::Unload(FontResource& resource) {
    destroyFont(resource.font);
    resource.atlasDynamic.destroy();
}

bool FontResourceLoader::IsLoaded() {
    return true;
}

void FontResourceLoader::Reload(FontResource& resource) {
    Unload(resource);
    Load(resource);
}
