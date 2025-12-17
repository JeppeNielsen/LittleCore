//
// Created by Jeppe Nielsen on 01/12/2025.
//

#include "FontResourceLoaderFactory.hpp"
#include "FileHelper.hpp"

using namespace LittleCore;

FontResourceLoaderFactory::Loader FontResourceLoaderFactory::Create() {
    return CreateLoader();
}

FontResourceLoaderFactory::~FontResourceLoaderFactory() {
}

bool FontResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return FileHelper::HasExtension(path, "ttf") ||
            FileHelper::HasExtension(path, "otf");
}
