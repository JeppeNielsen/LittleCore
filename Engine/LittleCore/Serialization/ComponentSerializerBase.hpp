//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include <tuple>

namespace LittleCore {
    struct IComponentSerializerBase {
        ~IComponentSerializerBase() = default;
    };

    template<typename TComponent, typename TSerializedComponent>
    struct ComponentSerializerBase : public IComponentSerializerBase {
        ~ComponentSerializerBase() = default;
        using Component = TComponent;
        using SerializedComponent = TSerializedComponent;
    };

    template <typename T>
    concept CustomSerializerPredicate = std::derived_from<T, IComponentSerializerBase>;
}