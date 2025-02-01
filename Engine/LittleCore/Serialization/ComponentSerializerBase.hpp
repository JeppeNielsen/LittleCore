//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include <tuple>

namespace LittleCore {

    template <typename T>
    concept CustomSerializer = requires(T t) {
        { t.CanSerialize() } -> std::same_as<void>;
    };

    template<typename TComponent, typename TSerializedComponent>
    struct ComponentSerializerBase {
        using Component = TComponent;
        using SerializedComponent = TSerializedComponent;

        void CanSerialize() {};
    };
}