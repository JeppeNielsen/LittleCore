//
// Created by Jeppe Nielsen on 30/12/2025.
//

#pragma once
#include "EntityGuiDrawer.hpp"
#include "MetaHelper.hpp"

#include "LocalTransform.hpp"
#include "Camera.hpp"
#include "Texturable.hpp"
#include "Renderable.hpp"
#include "Mesh.hpp"
#include "Prefab.hpp"
#include "PrefabExposedComponents.hpp"
#include "Label.hpp"
#include "Colorable.hpp"

namespace LittleCore {

    using DefaultDrawableTypes = Meta::TypeList<
            LocalTransform,
            Camera,
            Texturable,
            Renderable,
            Mesh,
            Prefab,
            PrefabExposedComponents,
            Label,
            Colorable
    >;

    template<typename ...T>
    using DefaultEntityGuiDrawer =
            Meta::Rebind<EntityGuiDrawer, Meta::Concat<DefaultDrawableTypes, Meta::TypeList<T...>>>;

}