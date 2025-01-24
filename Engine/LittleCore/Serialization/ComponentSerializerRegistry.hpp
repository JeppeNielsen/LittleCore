//
// Created by Jeppe Nielsen on 24/01/2025.
//

#pragma once
#include <nlohmann/json.hpp>
#include <tuple>
#include "TupleHelper.hpp"

namespace LittleCore {
    template<typename ...T>
    class ComponentSerializerRegistry {
    public:
        using json = nlohmann::json;

        using ComponentSerializers = std::tuple<T...>;

        template<typename TSerializer>
        TSerializer& Get() {
            return std::get<TSerializer>(componentSerializers);
        }

        template<typename TComponent>
        bool TrySerialize(json& json, const TComponent& component) {
            bool didSerialize = false;
            TupleHelper::for_each(componentSerializers, [&] (auto& type) {
                using ComponentType = typename decltype(type)::ComponentType;
                if constexpr (std::is_same_v<TComponent, ComponentType>) {
                    type.Serialize(json, component);
                    didSerialize = true;
                }
            });
            return didSerialize;
        }

    private:
        ComponentSerializers componentSerializers;
    };

    //Hej jeg hedder Jeppe og jeg kan skrive hurtigt.
    // iqp9r4yo
}