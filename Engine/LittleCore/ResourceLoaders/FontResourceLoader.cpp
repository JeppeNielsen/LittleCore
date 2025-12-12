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
}

void FontResourceLoader::Unload(FontResource& resource) {
    destroyFont(resource.font);
}

bool FontResourceLoader::IsLoaded() {
    return true;
}

void FontResourceLoader::Reload(FontResource& resource) {
    Unload(resource);
    Load(resource);
}
