//
// Created by Jeppe Nielsen on 25/02/2024.
//
#pragma once
#include <tuple>
#include "ResourceLoaderManager.hpp"

namespace LittleCore {

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

    template<typename ...T>
    struct ResourceManager {
        using Managers = std::tuple<ResourceLoaderManager<T>...>;
        using Resources = std::tuple<typename ResourceLoaderManager<T>::TResource ...>;
        Managers managers;

        template<typename TResource>
        ResourceHandle<TResource> Create(const std::string& id) {
            return std::get<
                    index_in_tuple<TResource, Resources>::value
            >(managers).Create(id);
        }

        template<typename TLoaderFactory, typename ...TArgs>
        void CreateLoaderFactory(TArgs&&... args) {
            std::get<ResourceLoaderManager<TLoaderFactory>>(managers).SetFactory(std::make_unique<TLoaderFactory>(std::forward<TArgs>(args)...));
        }

    };

}
