//
//  WorldBoundingBoxSystem.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once

#include <entt/entt.hpp>
#include "WorldTransform.hpp"
#include "LocalBoundingBox.hpp"
#include "WorldBoundingBox.hpp"

namespace LittleCore {
    class WorldBoundingBoxSystem  {
    public:
        WorldBoundingBoxSystem(entt::registry& registry);
        void Update();
    private:
        entt::registry& registry;
        entt::observer observer;
    };
}