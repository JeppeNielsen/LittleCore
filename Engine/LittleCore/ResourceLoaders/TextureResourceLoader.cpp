//
// Created by Jeppe Nielsen on 03/03/2024.
//

#include "TextureResourceLoader.hpp"
#include "ImageLoader.hpp"

using namespace LittleCore;

void TextureResourceLoader::Load(Texturable& texturable) {
    ImageLoader::TryLoadImage(path, [&texturable](unsigned char* data, int width, int height) {
        sg_image_desc desc{};
        desc.width = width;
        desc.height = height;
        desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        desc.usage = SG_USAGE_IMMUTABLE;
        desc.data.subimage[0][0] = {data, sizeof(unsigned char) * width * height * 4};
        texturable.texture = sg_make_image(&desc);
    });
}

void TextureResourceLoader::Unload(Texturable& texturable) {
    lc_sg_destroy(texturable.texture);
}

bool TextureResourceLoader::IsLoaded() {
    return true;
}

void TextureResourceLoader::Reload(Texturable& texturable) {
    Unload(texturable);
    Load(texturable);
}
