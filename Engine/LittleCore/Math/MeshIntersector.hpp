//
//  MeshIntersector.hpp
//  TinyEditor
//
//  Created by Jeppe Nielsen on 23/09/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#pragma once
#include "Math.hpp"
#include "Ray.hpp"
#include "Mesh.hpp"

namespace LittleCore {
    
    struct IntersectionResult {
        float distance;
        float barycentricU;
        float barycentricV;
        size_t triangleIndex;
        vec3 normal;
    };

    struct MeshIntersector {
        bool RayIntersectsTriangle(const Ray& ray,
        const vec3& tri0, const vec3& tri1, const vec3& tri2,
                                   float* pickDistance, float* barycentricU, float* barycentricV);
        bool TryIntersect(const Mesh& mesh, const Ray& ray, IntersectionResult& result);
    };
}
