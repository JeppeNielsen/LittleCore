//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include <memory>
#include "ResourceLoader.hpp"

namespace LittleCore {
    template<typename TLoader>
    struct IResourceLoaderFactory {
        using LoaderType = TLoader;
        using Loader = std::unique_ptr<TLoader>;
        virtual ~IResourceLoaderFactory() = default;

        template<typename ...Args>
        Loader CreateLoader(Args&&...args) {
            return std::make_unique<TLoader>(std::forward<Args>(args)...);
        }

        virtual Loader Create() = 0;
    };
}