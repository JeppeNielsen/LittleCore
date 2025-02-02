//
// Created by Jeppe Nielsen on 01/02/2025.
//

#pragma once
#include <string>
#include <vector>
#include "TypeUtility.hpp"
#include "ComponentSerializerBase.hpp"

namespace LittleCore {

    template<typename T>
    using SerializedComponentPtr = std::tuple<uint32_t, const T*>;

    template<typename T>
    using SerializedComponent = std::tuple<uint32_t, T>;

    template<typename T, typename TypeName>
    struct SerializedComponentPtrList {
        using ComponentType = T;
        std::string type = LittleCore::TypeUtility::GetClassName<TypeName>();
        std::vector <SerializedComponentPtr<T>> components;
    };

    template<typename T, typename TypeName>
    struct SerializedComponentList {
        using ComponentType = T;
        std::string type = LittleCore::TypeUtility::GetClassName<TypeName>();
        std::vector <SerializedComponent<T>> components;
    };

    template<typename ...T>
    struct SerializedRegistry {

        template<typename S>
        static constexpr auto Iterate() {
            if constexpr (CustomSerializerPredicate < S >) {
                return std::make_tuple(
                        SerializedComponentList<typename S::SerializedComponent, typename S::Component>());
            } else {
                return std::make_tuple(SerializedComponentPtrList<S, S>());
            }
        }

        static constexpr auto GetComponents() {
            return std::tuple_cat(Iterate<T>()...);
        }

        decltype(GetComponents()) components;
    };


}