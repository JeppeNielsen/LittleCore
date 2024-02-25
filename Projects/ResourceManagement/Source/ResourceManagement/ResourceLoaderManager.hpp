//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include "ResourceHandle.hpp"
#include "ResourceLoaderFactory.hpp"
#include <memory>

namespace LittleCore {

    template<typename T>
    struct ResourceLoaderManager {
        std::map<std::string, ResourceStorage<T>> storages;
        std::unique_ptr<IResourceLoaderFactory<T>> loaderFactory;

        void SetFactory(std::unique_ptr<IResourceLoaderFactory<T>>&& loaderFactory) {
            this->loaderFactory = std::move(loaderFactory);
        }

        ResourceHandle<T> Create(const std::string& id) {
            ResourceStorage<T>* storage;
            auto it = storages.find(id);
            if (it == storages.end()) {
                it = storages.emplace(id, loaderFactory->Create()).first;
            }
            storage = &it->second;
            return ResourceHandle<T>(storage);
        }


    };


}