//
//  Rect.hpp
//  Tiny
//
//  Created by Jeppe Nielsen on 26/07/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include <math.h>
#include "Math.hpp"

namespace LittleCore {
    struct Rect {
        vec2 min;
        vec2 max;
        
        inline bool Overlaps(const Rect& other) const {
            return max.x > other.min.x ||
                other.max.x < min.x ||
                max.y > other.max.y ||
                min.y < other.max.y;
        }
        
        inline bool operator ==(const Rect &other) const{
            return !(min!=other.min || max!=other.max);
        }
        
        inline  bool operator !=(const Rect &other) const{
            return (min!=other.min || max!=other.max);
        }
        
        inline vec2 Position() const {
            return min;
        }
        
        inline vec2 Size() const {
            return max - min;
        }
        
        inline vec2 Center() const {
            return (min + max) * 0.5f;
        }
        
        Rect CreateWorldAligned(const mat3x3& matrix) const {
            
            Rect rect;
            
            vec2 extends = max - min;
            
            vec2 halfExtends = extends * 0.5f;
            
            vec3 minVec3 = vec3(min, 1.0f);

            rect.min = matrix * minVec3; //matrix.TransformPoint(min);
            
            extends.x = (fabsf(matrix[0][0]) * halfExtends.x + fabsf(matrix[0][1]) * halfExtends.y);
            extends.y = (fabsf(matrix[1][0]) * halfExtends.x + fabsf(matrix[1][1]) * halfExtends.y);
            
            rect.max = min + extends * 2.0f;
            
            return rect;
        }
    };
}
