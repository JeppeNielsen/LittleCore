//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "TextureResourceLoader.hpp"
#include "ImageLoader.hpp"

using namespace LittleCore;
using namespace bgfx;

void TextureResourceLoader::Load(Texturable& texturable) {

    ImageLoader::TryLoadImage(path, [&texturable](unsigned char * data, int width, int height) {
        const bgfx::Memory* mem_image = bgfx::makeRef(data, sizeof(unsigned char)*width*height*4);
        texturable.texture = bgfx::createTexture2D(width, height, false, 0, TextureFormat::RGBA8, BGFX_TEXTURE_NONE, mem_image);
    });

}

void TextureResourceLoader::Unload(Texturable& texturable) {

    if (texturable.texture.idx != bgfx::kInvalidHandle) {
        bgfx::destroy(texturable.texture);
        texturable.texture = BGFX_INVALID_HANDLE;
    }
}

bool TextureResourceLoader::IsLoaded() {
    return true;
}

void TextureResourceLoader::Reload(Texturable& texturable) {
    Unload(texturable);
    Load(texturable);
}
