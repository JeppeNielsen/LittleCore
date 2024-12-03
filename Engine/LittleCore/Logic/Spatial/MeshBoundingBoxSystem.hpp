//
//  MeshBoundingBoxSystem.hpp
//  TinyOSX
//
//  Created by Jeppe Nielsen on 24/09/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//


#pragma once
#include "Mesh.hpp"
#include "LocalBoundingBox.hpp"
#include <entt/entt.hpp>

namespace LittleCore {
    class MeshBoundingBoxSystem {
    public:
        MeshBoundingBoxSystem(entt::registry& registry);

        void Update();
        
    private:
        entt::registry& registry;
        entt::observer observer;
    };
}
