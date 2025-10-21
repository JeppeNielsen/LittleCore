//
// Created by Jeppe Nielsen on 21/10/2025.
//

#pragma once

namespace LittleCore {

    namespace Meta {
        template<class... Ts>
        struct TypeList {

        };

        template<template<class...> class T, class List>
        struct RebindImpl;
        template<template<class...> class T, class... Ts>
        struct RebindImpl<T, TypeList<Ts...>> {
            using type = T<Ts...>;
        };

        template<template<class...> class T, class List>
        using Rebind = typename RebindImpl<T, List>::type;


        template<class List1, class List2>
        struct ConcatImpl;

        template<class... Ts, class... Us>
        struct ConcatImpl<TypeList<Ts...>, TypeList<Us...>> {
            using type = TypeList<Ts..., Us...>;
        };

        template<class L1, class L2>
        using Concat = typename ConcatImpl<L1, L2>::type;

    }
}