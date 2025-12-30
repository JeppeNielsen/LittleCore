//
// Created by Jeppe Nielsen on 30/12/2025.
//

#pragma once

#include <LocalBoundingBox.hpp>
#include "RegistrySerializer.hpp"
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
#include "LocalBoundingBox.hpp"
#include "WorldBoundingBox.hpp"

namespace LittleCore {

    using DefaultSerializableTypes = Meta::TypeList<
            LocalTransform,
            Hierarchy,
            Camera,
            Texturable,
            Renderable,
            Mesh,
            Prefab,
            PrefabExposedComponents,
            Label,
            Colorable,
            WorldTransform,
            LocalBoundingBox,
            WorldBoundingBox
    >;

    template<typename ...T>
    using DefaultRegistrySerializer =
            Meta::Rebind<RegistrySerializer, Meta::Concat<DefaultSerializableTypes, Meta::TypeList<T...>>>;

}