//
//  Camera.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Math.hpp"
#include "Ray.hpp"
#include "WorldTransform.hpp"
#include "Rect.hpp"

namespace LittleCore {
    struct Camera {

        float orthoSize = 1;
        float fieldOfView = 60;
        bool isOrthographic = false;
        
        float near;
        float far;

        Rect viewRect = {{0,0},{1,1}};

        bool clearBackground = true;
        uint32_t clearColor = 0x6495EDFF;

        mat4x4 GetProjection(float aspect) const;
     
        Ray GetRay(const WorldTransform &transform,
                   const ivec2& screenSize,
                   const ivec2& screenPosition) const;

        bool IsOrthographic() const;

    };
}
