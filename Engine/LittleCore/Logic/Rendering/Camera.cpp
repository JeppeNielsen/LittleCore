//
//  Camera.cpp
//  Tiny
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "Camera.hpp"
#include "Math.hpp"
#include "SokolDirect.hpp"

#include <algorithm>

using namespace LittleCore;

bool gethomogeneousDepth() {
    return true;
}

mat4x4 Camera::GetProjection(float aspect) const {

    if (!IsOrthographic()) {
       return glm::perspectiveLH(glm::radians(fieldOfView), aspect, near, far);
    } else {
        float xSize = orthoSize * 0.5f;
        float ySize = xSize * aspect;
        return glm::orthoLH( -xSize,xSize,-ySize,ySize, near, far);
    }
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
    const float minX = std::clamp(viewRect.min.x, 0.0f, 1.0f);
    const float minY = std::clamp(viewRect.min.y, 0.0f, 1.0f);
    const float maxX = std::clamp(viewRect.max.x, 0.0f, 1.0f);
    const float maxY = std::clamp(viewRect.max.y, 0.0f, 1.0f);

    const int viewportX = static_cast<int>(minX * screenSize.x);
    const int viewportY = static_cast<int>(minY * screenSize.y);
    const int viewportW = std::max(1, static_cast<int>((maxX - minX) * screenSize.x));
    const int viewportH = std::max(1, static_cast<int>((maxY - minY) * screenSize.y));

    float ndcX = ((screenPos.x - viewportX + 0.5f) / (float)viewportW) * 2.0f - 1.0f;
    float ndcY = ((screenPos.y - viewportY + 0.5f) / (float)viewportH) * 2.0f - 1.0f;
    ndcY = -ndcY;

    mat4 viewProjection = GetProjection((float)viewportW / (float)viewportH) * transform.worldInverse;
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

float Camera::GetDistance(const mat4x4& worldInverse, const mat4x4& world) const {

    const vec4 distanceToCameraPosition = vec4(
            world[3][0],
            world[3][1],
            world[3][2],
            1
    );

    const vec4 localPosition = worldInverse * distanceToCameraPosition;

    float distanceToCamera = localPosition.z;

    return distanceToCamera;
    //float fInvW = 1.0f / ( viewProjection[0][3] * distanceToCameraPosition.x + viewProjection[1][3] * distanceToCameraPosition.y + viewProjection[2][3] * distanceToCameraPosition.z + viewProjection[3][3] );
    //float distanceToCamera = ( viewProjection[0][2] * distanceToCameraPosition.x + viewProjection[1][2] * distanceToCameraPosition.y + viewProjection[2][2] * distanceToCameraPosition.z + viewProjection[3][2] ) * fInvW;
    //float fInvW = 1.0f / ( viewProjection[3][0] * distanceToCameraPosition.x + viewProjection[3][1] * distanceToCameraPosition.y + viewProjection[3][2] * distanceToCameraPosition.z + viewProjection[3][3] );
    //float distanceToCamera = ( viewProjection[2][0] * distanceToCameraPosition.x + viewProjection[2][1] * distanceToCameraPosition.y + viewProjection[2][2] * distanceToCameraPosition.z + viewProjection[2][3] ) * fInvW;

    //return distanceToCamera;
}
