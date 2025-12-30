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
#include "ResourceSerialization.hpp"
#include "MathReflection.hpp"

namespace LittleCore {

    template <typename T>
    concept StringLike =
    std::same_as<std::decay_t<T>, std::string> ||
    std::same_as<std::decay_t<T>, std::string_view> ||
    std::is_same_v<std::remove_cvref_t<T>, char*> ||
    std::is_same_v<std::remove_cvref_t<T>, const char*>;

    template <typename T>
    concept SimpleType =
    std::is_arithmetic_v<std::remove_cvref_t<T>> ||
    std::is_enum_v<std::remove_cvref_t<T>>       ||
    std::same_as<std::remove_cvref_t<T>, std::string>;

    // primary: false
    template <class T>
    struct is_glz_custom_t : std::false_type {};

// match glaze proxy
    template <class T, class Read, class Write>
    struct is_glz_custom_t<glz::custom_t<T, Read, Write>> : std::true_type {
        using type = T;
    };

    template <class T>
    inline constexpr bool is_glz_custom_t_v =
            is_glz_custom_t<std::remove_cvref_t<T>>::value;

    template <class T>
    concept IsGlzCustom = is_glz_custom_t_v<T>;

    class ReflectionUtility {
    public:
        template<typename T, typename Callable>
        static void IterateMembers(T&& object, Callable&& callable) {

            if constexpr (IsGlzCustom<T>) {
                using innerType = std::remove_cvref_t<decltype(object.val)>;

                if constexpr (DerivedFromResourceComponent<innerType>) {
                    //IterateMembers(object.val.handle, callable);
                    callable("handle", object.val.handle);
                }
            }
            else {

                decltype(glz::reflect<T>()) fieldNames;

                int memberIndex = 0;
                glz::for_each_field(object, [&, callable](auto&& value) {
                    using valueType = std::remove_cvref_t<decltype(value)>;

                    if constexpr (StringLike<valueType>) {
                        callable(fieldNames.keys[memberIndex], value);
                    } else if constexpr (SimpleType<valueType>) {
                        callable(fieldNames.keys[memberIndex], value);
                    } else if constexpr (glz::vector_like<valueType>) {

                        for (auto& v: value) {
                            using containedType = std::remove_cvref_t<decltype(v)>;

                            if constexpr (SimpleType<containedType>) {
                                callable(fieldNames.keys[memberIndex], v);
                            } else {
                                IterateMembers<decltype(v)>(v, callable);
                            }
                        }

                    } else {
                        bool recurse = callable(fieldNames.keys[memberIndex], value);
                        if (recurse) {
                            IterateMembers(value, callable);
                        }
                    }
                    memberIndex++;
                });
            }
        }





    };





}

