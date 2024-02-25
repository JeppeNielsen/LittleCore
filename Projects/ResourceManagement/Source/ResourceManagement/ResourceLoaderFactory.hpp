//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include <memory>
#include "ResourceLoader.hpp"

namespace LittleCore {
    template<typename T>
    struct IResourceLoaderFactory {
        virtual ~IResourceLoaderFactory() = default;
        virtual std::unique_ptr<IResourceLoader<T>> Create() = 0;
    };
}