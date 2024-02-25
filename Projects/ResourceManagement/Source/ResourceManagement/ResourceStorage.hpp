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

        explicit ResourceStorage(ResourceLoader&& loader) : loader(std::move(loader)) {
            std::cout << "Resource created \n";
        }

        ~ResourceStorage()  {
            std::cout << "Resource freed \n";
        }

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

            std::cout << "IncrementReference, ReferenceCount: " << referenceCount << "\n";
        }

        void DecrementReference() {
            --referenceCount;

            if (referenceCount == 0) {
                loader->Unload(resource);
            }

            std::cout << "DecrementReference, ReferenceCount: " << referenceCount << "\n";
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