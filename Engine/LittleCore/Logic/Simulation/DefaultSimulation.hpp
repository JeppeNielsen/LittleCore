//
// Created by Jeppe Nielsen on 04/12/2024.
//

#pragma once

#include "Simulation.hpp"
#include "SokolRenderer.hpp"
#include <InputSystem.hpp>
#include "HierarchySystem.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include "MeshBoundingBoxSystem.hpp"
#include "WorldTransformSystem.hpp"
#include "RenderSystem.hpp"
#include "TexturableSystem.hpp"
#include "ColorableSystem.hpp"
#include "LabelMeshSystem.hpp"
#include "PrefabSystem.hpp"
#include "LayoutSystem.hpp"
#include "SpriteMeshSystem.hpp"

namespace LittleCore {

    using DefaultInputSystems = InputSystems<InputSystem>;

    using DefaultUpdateSystems = UpdateSystems<
            HierarchySystem,
            LayoutSystem,
            WorldTransformSystem,
            WorldBoundingBoxSystem,
            MeshBoundingBoxSystem,
            TexturableSystem,
            ColorableSystem,
            LabelMeshSystem,
            SpriteMeshSystem,
            PrefabSystem>;

    using DefaultRenderSystems = RenderSystems<RenderSystem>;

    using DefaultSimulation = Simulation<DefaultInputSystems, DefaultUpdateSystems, DefaultRenderSystems>;

    template<typename ...T>
    using CustomUpdateSystems = ConcatSimulationSystemLists<DefaultUpdateSystems, UpdateSystems<T...>>;

    template<typename ...T>
    using CustomSimulation = Simulation<DefaultInputSystems, CustomUpdateSystems<T...>, DefaultRenderSystems>;

}
