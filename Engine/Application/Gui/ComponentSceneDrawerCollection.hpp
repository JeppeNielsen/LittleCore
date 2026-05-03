//
// Created by Codex on 02/05/2026.
//

#pragma once

#include <tuple>
#include "TupleHelper.hpp"

namespace LittleCore {

    struct SceneDrawerContext;

    class ComponentSceneDrawerBase {
    public:
        virtual ~ComponentSceneDrawerBase() = default;
        virtual void Draw(SceneDrawerContext& context) = 0;
    };

    template<typename ...T>
    class ComponentSceneDrawerCollection : public ComponentSceneDrawerBase {
    public:
        void Draw(SceneDrawerContext& context) override {
            TupleHelper::for_each(drawers, [&context](auto& drawer) {
                drawer.TryDraw(context);
            });
        }

        template<typename TDrawer>
        TDrawer& GetDrawer() {
            return std::get<TDrawer>(drawers);
        }

    private:
        std::tuple<T...> drawers;
    };

    template<typename...>
    struct ConcatComponentSceneDrawerCollection_impl;

    template<typename... s1, typename... s2>
    struct ConcatComponentSceneDrawerCollection_impl<ComponentSceneDrawerCollection<s1...>, ComponentSceneDrawerCollection<s2...>> {
        using type = ComponentSceneDrawerCollection<s1..., s2...>;
    };

    template<typename A1, typename A2>
    using ConcatComponentSceneDrawerCollections = typename ConcatComponentSceneDrawerCollection_impl<A1, A2>::type;
}
