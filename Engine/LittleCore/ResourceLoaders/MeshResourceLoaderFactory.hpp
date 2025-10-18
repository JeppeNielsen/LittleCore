//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include "ResourceLoaderFactory.hpp"
#include "MeshResourceLoader.hpp"

namespace LittleCore {

    struct MeshResourceLoaderFactory : IResourceLoaderFactory<MeshResourceLoader> {
        Loader Create() {
            return CreateLoader();
        }
    };


}