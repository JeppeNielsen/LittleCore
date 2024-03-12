//
// Created by Jeppe Nielsen on 12/03/2024.
//

#pragma once
#include <tuple>
#include <utility>

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




    };



}