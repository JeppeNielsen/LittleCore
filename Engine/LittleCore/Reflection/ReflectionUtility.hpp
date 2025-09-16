//
// Created by Jeppe Nielsen on 16/09/2025.
//
#pragma once
#include <glaze/glaze.hpp>
#include <glaze/concepts/container_concepts.hpp>
#include <iostream>
#include <utility>
#include "TupleHelper.hpp"
#include <concepts>
#include "TypeUtility.hpp"

namespace LittleCore {

    template <typename T>
    concept StringLike =
    std::same_as<std::decay_t<T>, std::string> ||
    std::same_as<std::decay_t<T>, std::string_view> ||
    std::is_same_v<std::remove_cvref_t<T>, char*> ||
    std::is_same_v<std::remove_cvref_t<T>, const char*>;

    template <typename T>
    concept SimpleType = std::is_arithmetic_v<T> || std::is_enum_v<T>;

    class ReflectionUtility {
    public:
        template<typename T, typename Callable>
        static void IterateMembers(T&& object, Callable&& callable) {

            decltype(glz::reflect<T>()) fieldNames;

            int memberIndex = 0;
            glz::for_each_field(object, [&, callable](auto& value) {
                using valueType = std::remove_cvref_t<decltype(value)>;

                if constexpr (StringLike<valueType>) {
                    callable(fieldNames.keys[memberIndex], value);
                } else if constexpr (SimpleType<valueType>) {
                    callable(fieldNames.keys[memberIndex], value);
                } else if constexpr (glz::detail::vector_like<valueType>) {

                    for(auto& v : value) {
                        using containedType = std::remove_cvref_t<decltype(v)>;

                        if constexpr (SimpleType<containedType>) {
                            callable(fieldNames.keys[memberIndex], v);
                        } else {
                            IterateMembers<decltype(v)>(v, callable);
                        }
                    }

                } else {
                    callable(fieldNames.keys[memberIndex], value);
                    IterateMembers(value, callable);
                }
                memberIndex++;
            });

        }





    };





}

