//
//  Camera.cpp
//  Tiny
//
//  Created by Jeppe Nielsen on 27/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "Camera.hpp"

using namespace LittleCore;

mat4x4 Camera::GetProjection() const {
   return ortho(-ViewSize.x, ViewSize.x, -ViewSize.y, ViewSize.y, Near, Far);
   //return ortho(0.0f, ViewSize.x, 0.0f, ViewSize.y, Near, Far);
}

Ray Camera::GetRay(const WorldTransform &transform, const ivec2& screenSize, const ivec2& screenPosition) const {
    
    const ivec2 center = screenSize / 2;
    
    vec2 fromCenter = screenPosition - center;
    fromCenter /= center;
    
    mat4x4 viewProjection = GetProjection() * transform.worldInverse;
    viewProjection = inverse(viewProjection);
    
    vec4 rayStartPosition = vec4(fromCenter.x,fromCenter.y,-1.0f,1.0f);
    vec4 rayEndPosition = vec4(fromCenter.x,fromCenter.y,1.0f,1.0f);
    
    rayStartPosition = viewProjection * rayStartPosition;
    rayEndPosition = viewProjection * rayEndPosition;
    
    return Ray(rayStartPosition, rayEndPosition - rayStartPosition);
}
