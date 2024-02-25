//
// Created by Jeppe Nielsen on 25/02/2024.
//
#pragma once
#include <tuple>
#include "ResourceLoaderManager.hpp"

namespace LittleCore {

    template<typename ...T>
    struct ResourceManager {
        using Managers = std::tuple<ResourceLoaderManager<T>...>;
        Managers managers;

        template<typename TResource>
        ResourceHandle<TResource> Create(const std::string& id) {
            return std::get<ResourceLoaderManager<TResource>>(managers).Create(id);
        }

        template<typename TResource, typename TLoader, typename ...TArgs>
        void SetLoaderFactory(TArgs&&... args) {
            std::get<ResourceLoaderManager<TResource>>(managers).SetFactory(std::make_unique<TLoader>(std::forward<TArgs>(args)...));
        }

    };

}
