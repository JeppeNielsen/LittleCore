#pragma once
#include "Math.hpp"

namespace LittleCore {
    class Ray;
	class Plane {
	public:
        Plane();
		Plane(const vec3& normal, float d);
        Plane(const vec3& normal, const vec3& position);
        Plane(const vec3& p0, const vec3& p1, const vec3& p2);
		
		float Distance(const vec3& position) const;

		void Normalize();
		void Flip();
    
        bool IntersectsRay(const Ray& ray, float* distance) const;

		vec3 normal;
		float d;
	};
}

