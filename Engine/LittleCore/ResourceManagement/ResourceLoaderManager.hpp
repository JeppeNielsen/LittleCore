//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include "ResourceHandle.hpp"
#include "ResourceLoaderFactory.hpp"
#include <memory>

namespace LittleCore {

    template<typename TLoaderFactory>
    struct ResourceLoaderManager {
        using TResource = typename TLoaderFactory::LoaderType::Type;
        std::map<std::string, ResourceStorage<TResource>> storages;
        std::unique_ptr<TLoaderFactory> loaderFactory;

        void SetFactory(std::unique_ptr<TLoaderFactory>&& loaderFactory) {
            this->loaderFactory = std::move(loaderFactory);
        }

        ResourceHandle<TResource> Create(const std::string& id) {
            assert(loaderFactory!= nullptr);
            ResourceStorage<TResource>* storage;
            auto it = storages.find(id);
            if (it == storages.end()) {
                it = storages.emplace(id, loaderFactory->Create()).first;
            }
            storage = &it->second;
            return ResourceHandle<TResource>(storage);
        }


    };


}