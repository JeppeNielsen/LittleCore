//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once

namespace LittleCore {
    template<typename T>
    struct IResourceLoader {
        using Type = T;
        virtual ~IResourceLoader() = default;
        virtual void Load(T &resource) = 0;
        virtual void Unload(T &resource) = 0;
        virtual bool IsLoaded() = 0;
    };
}