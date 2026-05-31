//
// Created by Jeppe Nielsen on 29/05/2026.
//

#pragma once
#include "OctreeSystem.hpp"
#include "Clickable.hpp"
#include "WorldTransform.hpp"

namespace LittleCore {
    using ClickableOctreeSystem = OctreeSystem<Clickable, WorldTransform>;
}
