//
//  Ray.cpp
//  PocketEngine
//
//  Created by Jeppe Nielsen on 3/16/13.
//  Copyright (c) 2013 Jeppe Nielsen. All rights reserved.
//

#include "Ray.hpp"
#include "Math.hpp"
#include "BoundingBox.hpp"
#include <cmath>

using namespace LittleCore;

void Ray::Transform(const mat4x4 &matrix) {
    position = matrix * vec4(position,1.0f); //matrix.TransformPosition(position);
    direction = matrix * vec4(direction, 0.0f); //matrix.TransformVector(direction);
}

bool Ray::Intersect(const BoundingBox& box) const {
    vec3 diff;
    
	diff.x = position.x - box.center.x;
	if(fabsf(diff.x)>box.extends.x && diff.x*direction.x>=0.0f)	return false;
    
	diff.y = position.y - box.center.y;
	if(fabsf(diff.y)>box.extends.y && diff.y*direction.y>=0.0f)	return false;
    
    diff.z = position.z - box.center.z;
	if(fabsf(diff.z)>box.extends.z && diff.z*direction.z>=0.0f)	return false;
    
	float fAWdU[3];
	fAWdU[0] = fabsf(direction.x);
	fAWdU[1] = fabsf(direction.y);
	fAWdU[2] = fabsf(direction.z);
    
	float f;
	f = direction.y * diff.z - direction.z * diff.y;	if(fabsf(f)>box.extends.y*fAWdU[2] + box.extends.z*fAWdU[1])	return false;
	f = direction.z * diff.x - direction.x * diff.z;	if(fabsf(f)>box.extends.x*fAWdU[2] + box.extends.z*fAWdU[0])	return false;
	f = direction.x * diff.y - direction.y * diff.x;	if(fabsf(f)>box.extends.x*fAWdU[1] + box.extends.y*fAWdU[0])	return false;
    
	return true;    
}
