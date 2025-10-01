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

bool gethomogeneousDepth() {
    //return bgfx::getCaps()->homogeneousDepth;
    return true;
}

mat4x4 Camera::GetProjection(float aspect) const {

    //float proj[16];
    //bx::mtxProj(proj, 60.0f, 1.0f, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
    //return glm::make_mat4x4(proj);

   //return ortho(-ViewSize.x, ViewSize.x, -ViewSize.y, ViewSize.y, Near, Far);
   //return ortho(0.0f, ViewSize.x, 0.0f, ViewSize.y, Near, Far);

    float proj[16];

    if (!IsOrthographic()) {
        bx::mtxProj(proj, fieldOfView, aspect, near, far, gethomogeneousDepth());
    } else {
        float xSize = orthoSize * 0.5f;
        float ySize = xSize * aspect;
        bx::mtxOrtho(proj, -xSize,xSize,-ySize,ySize, near, far, 0,gethomogeneousDepth());
    }
    return glm::make_mat4x4(proj);
}

/*
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
 */


Ray Camera::GetRay(const WorldTransform& transform,
                   const ivec2& screenSize,
                   const ivec2& screenPos) const
{
    float ndcX = ((screenPos.x + 0.5f) / (float)screenSize.x) * 2.0f - 1.0f;
    float ndcY = ((screenPos.y + 0.5f) / (float)screenSize.y) * 2.0f - 1.0f;
    ndcY = -ndcY;

    mat4 viewProjection = GetProjection((float)screenSize.x / (float)screenSize.y) * transform.worldInverse;
    mat4 inverseViewProjection = inverse(viewProjection);

    vec4 pNear = vec4(ndcX, ndcY, -1.0f, 1.0f);
    vec4 pFar  = vec4(ndcX, ndcY,  1.0f, 1.0f);

    pNear = inverseViewProjection * pNear; pNear /= pNear.w;
    pFar  = inverseViewProjection * pFar; pFar  /= pFar.w;

    vec3 origin = vec3(pNear);
    //vec3 dir = normalize(vec3(pFar - pNear));
    vec3 dir = vec3(pFar - pNear);
    return Ray(origin, dir);
}

bool Camera::IsOrthographic() const {
    return isOrthographic;
}
