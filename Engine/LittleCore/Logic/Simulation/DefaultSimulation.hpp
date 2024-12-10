//
// Created by Jeppe Nielsen on 04/12/2024.
//

#pragma once

#include "Simulation.hpp"
#include "BgfxRenderer.hpp"
#include <InputSystem.hpp>
#include "HierarchySystem.hpp"
#include "WorldBoundingBoxSystem.hpp"
#include "MeshBoundingBoxSystem.hpp"
#include "WorldTransformSystem.hpp"
#include "RenderSystem.hpp"

namespace LittleCore {

    using DefaultInputSystems = InputSystems<InputSystem>;

    using DefaultUpdateSystems = UpdateSystems<
            HierarchySystem,
            MeshBoundingBoxSystem,
            WorldBoundingBoxSystem,
            WorldTransformSystem>;

    using DefaultRenderSystems = RenderSystems<RenderSystem>;

    using DefaultSimulation = Simulation<DefaultInputSystems, DefaultUpdateSystems, DefaultRenderSystems>;

}

