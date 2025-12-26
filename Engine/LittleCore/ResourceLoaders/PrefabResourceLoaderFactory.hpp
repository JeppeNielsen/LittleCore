//
// Created by Jeppe Nielsen on 25/10/2025.
//


#pragma once
#include "ResourceLoaderFactory.hpp"
#include "PrefabResourceLoader.hpp"
#include "RegistrySerializerBase.hpp"

namespace LittleCore {

    struct PrefabResourceLoaderFactory  : IResourceLoaderFactory<PrefabResourceLoader> {
    public:

        PrefabResourceLoaderFactory(RegistrySerializerBase& registrySerializer, void* defaultResourceManager);

        Loader Create() override;
        bool IsPathSupported(const std::string& path);

    private:
        RegistrySerializerBase& registrySerializer;
        void* defaultResourceManager;
    };
}
