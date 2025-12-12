//
// Created by Jeppe Nielsen on 01/12/2025.
//

#include "FontResourceLoaderFactory.hpp"
#include "FileHelper.hpp"
#include <msdfgen.h>
#include <msdfgen-ext.h>

using namespace LittleCore;
using namespace msdfgen;

FontResourceLoaderFactory::Loader FontResourceLoaderFactory::Create() {
    if (freeType == nullptr) {
        freeType = initializeFreetype();
    }

    return CreateLoader(freeType);
}

FontResourceLoaderFactory::~FontResourceLoaderFactory() {
    if (freeType) {
        deinitializeFreetype(freeType);
        freeType = nullptr;
    }
}

bool FontResourceLoaderFactory::IsPathSupported(const std::string& path) {
    return FileHelper::HasExtension(path, "ttf") ||
            FileHelper::HasExtension(path, "otf");
}
