//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include <tuple>
#include <utility>
#include <type_traits>

namespace LittleCore {

    namespace TupleHelper {

        template< size_t I, typename T, typename Tuple_t>
        constexpr size_t index_in_tuple_fn(){
            static_assert(I < std::tuple_size<Tuple_t>::value,"The element is not in the tuple");

            typedef typename std::tuple_element<I,Tuple_t>::type el;
            if constexpr(std::is_same<T,el>::value ){
                return I;
            }else{
                return index_in_tuple_fn<I+1,T,Tuple_t>();
            }
        }

        template<typename T, typename Tuple_t>
        struct index_in_tuple{
            static constexpr size_t value = index_in_tuple_fn<0,T,Tuple_t>();
        };

        template<std::size_t I = 0, typename FuncT, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), void>::type
        for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
        { }

        template<std::size_t I = 0, typename FuncT, typename... Tp>
        inline typename std::enable_if<I < sizeof...(Tp), void>::type
        for_each(std::tuple<Tp...>& t, FuncT f)
        {
            f(std::get<I>(t));
            for_each<I + 1, FuncT, Tp...>(t, f);
        }

        template <typename T, typename Tuple>
        constexpr bool has_type_in_tuple = false;  // Base case

        template <typename T, typename... Ts>
        constexpr bool has_type_in_tuple<T, std::tuple<Ts...>> = (std::same_as<T, Ts> || ...);
// contains_in_tuple<T, std::tuple<...>>
        template<class T, class Tuple>
        struct contains_in_tuple;

        template<class T, class... Bs>
        struct contains_in_tuple<T, std::tuple<Bs...>>
                : std::bool_constant<(std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>,
                        std::remove_cv_t<std::remove_reference_t<Bs>>> || ...)> {};

        template<class T, class Tuple>
        inline constexpr bool contains_in_tuple_v = contains_in_tuple<T, Tuple>::value;

        template<bool Keep, class X>
        constexpr auto keep_or_empty(X&& x) {
            if constexpr (Keep) {
                return std::tuple<std::remove_cvref_t<X>>(std::forward<X>(x));
            } else {
                return std::tuple<>{};
            }
        }

// ---- A \ B (by TYPE) ----
        template<class TupleA, class TupleB, std::size_t... I>
        constexpr auto exclude_impl(TupleA&& a, const TupleB&, std::index_sequence<I...>) {
            using A = std::remove_reference_t<TupleA>;
            return std::tuple_cat(
                    keep_or_empty<!contains_in_tuple_v<std::remove_cvref_t<std::tuple_element_t<I, A>>, TupleB>>(
                            std::get<I>(std::forward<TupleA>(a))
                    )...
            );
        }

        template<class TupleA, class TupleB>
        constexpr auto exclude(TupleA&& a, const TupleB& b) {
            using A = std::remove_reference_t<TupleA>;
            (void)b; // type-only
            return exclude_impl(std::forward<TupleA>(a), b,
                                std::make_index_sequence<std::tuple_size_v<A>>{});
        }


    };



}