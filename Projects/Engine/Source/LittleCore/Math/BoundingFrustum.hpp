#pragma once
#include "Math.hpp"
#include "Plane.hpp"
#include "BoundingBox.hpp"

namespace LittleCore {
	class BoundingFrustum {
    public:
        BoundingFrustum();
        
		void SetFromViewProjection(const mat4x4& viewProjection);

		enum Intersection {
			INTERSECT = 0,
			INSIDE = 1,
			OUTSIDE = 2,
		};

		static int Count;

		Intersection Intersect(const BoundingBox& box) const;

		Plane planes[6];
	};
}

