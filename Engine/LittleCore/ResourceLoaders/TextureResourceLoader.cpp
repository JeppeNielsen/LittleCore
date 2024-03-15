//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "TextureResourceLoader.hpp"
#include "ImageLoader.hpp"

using namespace LittleCore;
using namespace bgfx;

void TextureResourceLoader::Load(TextureResource &resource) {

    ImageLoader::TryLoadImage(path, [&resource](unsigned char * data, int width, int height) {
        //const bgfx::Memory* mem_image = bgfx::makeRef(data, sizeof(unsigned char)*width*height);
        resource.handle = bgfx::createTexture2D(width, height, false, 0, TextureFormat::RGBA8, BGFX_TEXTURE_NONE|BGFX_SAMPLER_POINT);
    });

}

void TextureResourceLoader::Unload(TextureResource &resource) {

    if (resource.handle.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(resource.handle);
        resource.handle = BGFX_INVALID_HANDLE;
    }
}

bool TextureResourceLoader::IsLoaded() {
    return true;
}
