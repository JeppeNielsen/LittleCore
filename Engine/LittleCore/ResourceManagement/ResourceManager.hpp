//
// Created by Jeppe Nielsen on 25/02/2024.
//
#pragma once
#include "TupleHelper.hpp"
#include "ResourceLoaderManager.hpp"

namespace LittleCore {



    template<typename ...T>
    struct ResourceManager {
        ResourcePathMapper& pathMapper;
        using Managers = std::tuple<ResourceLoaderManager<T>...>;
        using Resources = std::tuple<typename ResourceLoaderManager<T>::TResource ...>;
        Managers managers;

        ResourceManager(ResourcePathMapper& pathMapper) : pathMapper(pathMapper) {
            TupleHelper::for_each(managers, [&pathMapper](auto& manager) {
                manager.pathMapper = &pathMapper;
            });
        }



        template<typename TResource>
        ResourceHandle<TResource> Create(const std::string& id) {
            return std::get<
                    TupleHelper::index_in_tuple<TResource, Resources>::value
            >(managers).Create(id);
        }

        template<typename TLoaderFactory, typename ...TArgs>
        void CreateLoaderFactory(TArgs&&... args) {
            std::get<ResourceLoaderManager<TLoaderFactory>>(managers).SetFactory(std::make_unique<TLoaderFactory>(std::forward<TArgs>(args)...));
        }

    };

}
