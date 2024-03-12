//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include <memory>
#include "ResourceLoader.hpp"

namespace LittleCore {
    template<typename T>
    struct ResourceStorage {
        using ResourceLoader = std::unique_ptr<IResourceLoader<T>>;
        ResourceLoader loader;
        T resource;

        ResourceStorage(ResourceLoader& loader) : loader(std::move(loader)) { }

        ~ResourceStorage()  = default;

        ResourceStorage(ResourceStorage&&) = delete;
        ResourceStorage(const ResourceStorage&) = delete;
        ResourceStorage& operator=(const ResourceStorage&) = delete;
        ResourceStorage& operator=(ResourceStorage&&) = delete;

        int referenceCount = 0;

        void IncrementReference() {
            ++referenceCount;
            if (referenceCount == 1) {
                loader->Load(resource);
            }
        }

        void DecrementReference() {
            --referenceCount;
            if (referenceCount == 0) {
                loader->Unload(resource);
            }
        }

        T* GetResource() {
            return &resource;
        }

        void Update() {

        }

        bool IsLoaded() {
            return loader->IsLoaded();
        }
    };


}