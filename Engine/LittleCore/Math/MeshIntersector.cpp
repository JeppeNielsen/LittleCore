//
//  MeshIntersector.cpp
//  TinyEditor
//
//  Created by Jeppe Nielsen on 23/09/2020.
//  Copyright © 2020 Jeppe Nielsen. All rights reserved.
//

#include "MeshIntersector.hpp"

using namespace LittleCore;


bool MeshIntersector::RayIntersectsTriangle(const Ray& ray,
                             const vec3& tri0, const vec3& tri1, const vec3& tri2,
                             float* pickDistance, float* barycentricU, float* barycentricV) {
    
    // Find vectors for two edges sharing vert0
    vec3 edge1 = tri1 - tri0;
    vec3 edge2 = tri2 - tri0;
    
    // Begin calculating determinant - also used to calculate barycentricU parameter
    vec3 pvec = cross(ray.direction, edge2);
    
    // If determinant is near zero, ray lies in plane of triangle
    float det = dot(edge1, pvec);
    if (det < 0.0001f)
        return false;
    
    // Calculate distance from vert0 to ray origin
    vec3 tvec = ray.position - tri0;
    
    // Calculate barycentricU parameter and test bounds
    *barycentricU = dot(tvec, pvec); //tvec.Dot(pvec);
    if (*barycentricU < 0.0f || *barycentricU > det)
        return false;
    
    // Prepare to test barycentricV parameter
    vec3 qvec = cross(tvec, edge1);
    
    // Calculate barycentricV parameter and test bounds
    *barycentricV = dot(ray.direction, qvec);
    if (*barycentricV < 0.0f || *barycentricU + *barycentricV > det)
        return false;
    
    // Calculate pickDistance, scale parameters, ray intersects triangle
    *pickDistance = dot(edge2, qvec);
    float fInvDet = 1.0f / det;
    *pickDistance *= fInvDet;
    *barycentricU *= fInvDet;
    *barycentricV *= fInvDet;
    
    return true;
}


bool MeshIntersector::TryIntersect(const Mesh& mesh, const Ray &ray, IntersectionResult &result) {
    const auto& triangles = mesh.triangles;
    if (triangles.empty()) return false;
    
    const auto& vertices = mesh.vertices;
    
    float minDistance = 10000000.0f;
        
    bool hit = false;
    size_t foundTriangleIndex = 0;
    for (size_t i=0; i<triangles.size(); i+=3) {
        
        if (RayIntersectsTriangle(ray,
                                  vertices[triangles[i]].position, vertices[triangles[i+1]].position, vertices[triangles[i+2]].position,
                                  &result.distance, &result.barycentricU, &result.barycentricV)) {
        
            if (result.distance<minDistance) {
                minDistance = result.distance;
                foundTriangleIndex = i;
            }
            hit = true;
        }
    }
    
    if (hit) {
        vec3 v0 = vertices[triangles[foundTriangleIndex]].position;
        vec3 v1 = vertices[triangles[foundTriangleIndex+1]].position;
        vec3 v2 = vertices[triangles[foundTriangleIndex+2]].position;
        
        vec3 tan1 = v1 - v0;
        vec3 tan2 = v2 - v0;
    
        result.normal = cross(tan1, tan2);
        result.normal = {0,0,1};//normalize(result.normal);
    }
    
    return hit;
}
