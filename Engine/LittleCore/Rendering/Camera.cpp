//
//  Camera.cpp
//  Tiny
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "Camera.hpp"
#include "Math.hpp"
#include "bgfx/bgfx.h"
#include <bx/math.h>

using namespace LittleCore;

mat4x4 Camera::GetProjection(float aspect) const {

    //float proj[16];
    //bx::mtxProj(proj, 60.0f, 1.0f, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    //return glm::make_mat4x4(proj);

   //return ortho(-ViewSize.x, ViewSize.x, -ViewSize.y, ViewSize.y, Near, Far);
   //return ortho(0.0f, ViewSize.x, 0.0f, ViewSize.y, Near, Far);

    float proj[16];

    if (orthoSize <=0.00001f) {
        bx::mtxProj(proj, fieldOfView, aspect, near, far, bgfx::getCaps()->homogeneousDepth);
    } else {
        float xSize = orthoSize * 0.5f;
        float ySize = xSize * aspect;
        bx::mtxOrtho(proj, -xSize,xSize,-ySize,ySize, near, far, 0,bgfx::getCaps()->homogeneousDepth);
    }
    return glm::make_mat4x4(proj);
}

Ray Camera::GetRay(const WorldTransform &transform, const ivec2& screenSize, const ivec2& screenPosition) const {
    
    const ivec2 center = screenSize / 2;
    
    vec2 fromCenter = screenPosition - center;
    fromCenter /= center;
    
    mat4x4 viewProjection = GetProjection(screenSize.y / (float)screenSize.x) * transform.worldInverse;
    viewProjection = inverse(viewProjection);
    
    vec4 rayStartPosition = vec4(fromCenter.x,fromCenter.y,-1.0f,1.0f);
    vec4 rayEndPosition = vec4(fromCenter.x,fromCenter.y,1.0f,1.0f);
    
    rayStartPosition = viewProjection * rayStartPosition;
    rayEndPosition = viewProjection * rayEndPosition;
    
    return Ray(rayStartPosition, rayEndPosition - rayStartPosition);
}
