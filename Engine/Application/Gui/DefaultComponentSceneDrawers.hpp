//
// Created by Codex on 02/05/2026.
//

#pragma once

#include "ComponentSceneDrawerCollection.hpp"
#include "MetaHelper.hpp"
#include "SceneDrawers/LocalTransformSceneDrawer.hpp"
#include "SceneDrawers/CameraSceneDrawer.hpp"
#include "SceneDrawers/SizableSceneDrawer.hpp"

namespace LittleCore {

    using DefaultComponentSceneDrawerTypes = Meta::TypeList<
            LocalTransformSceneDrawer,
            CameraSceneDrawer,
            SizableSceneDrawer
    >;

    template<typename ...T>
    using DefaultComponentSceneDrawers =
            Meta::Rebind<ComponentSceneDrawerCollection, Meta::Concat<DefaultComponentSceneDrawerTypes, Meta::TypeList<T...>>>;
}
