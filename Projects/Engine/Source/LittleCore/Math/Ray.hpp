//
//  Ray.h
//  PocketEngine
//
//  Created by Jeppe Nielsen on 3/16/13.
//  Copyright (c) 2013 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Math.hpp"

namespace LittleCore {
    class BoundingBox;
    class Ray {
    public:
        Ray() {}
        ~Ray() {}
        
        Ray(const Ray& ray) {
            position = ray.position;
            direction = ray.direction;
        }

        Ray(const vec3& position, const vec3& direction) {
            this->position = position;
            this->direction = direction;
        }

        vec3 GetPosition(float distance) const {
            return position + direction * distance;
        }
                
        void Transform(const mat4x4 &matrix);
        bool Intersect(const BoundingBox& box) const;
        
        vec3 position;
        vec3 direction;
    };
}
