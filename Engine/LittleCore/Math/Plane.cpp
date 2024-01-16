#include "Plane.hpp"
#include "Ray.hpp"

using namespace LittleCore;

Plane::Plane() {}

Plane::Plane(const vec3& normal, float d) : normal(normal), d(d) {
}

Plane::Plane(const vec3& p0, const vec3& p1, const vec3& p2) {
    vec3 p01 = p0 - p1;
    vec3 p02 = p0 - p2;
    normal = cross(p01, p02);
    d = dot(normal, p0);
}   

Plane::Plane(const vec3& normal, const vec3& position) {
    this->normal = normal;
    d = dot(position, normal);
}

float Plane::Distance(const vec3& position) const {
	return position.x * normal.x + position.y * normal.y + position.z * normal.z + d;
}

void Plane::Normalize() {
	float lengthInv = 1.0f / length(normal);
	normal.x *= lengthInv;
	normal.y *= lengthInv;
	normal.z *= lengthInv;
	d *= lengthInv;
}

void Plane::Flip() {
	normal.x = -normal.x;
	normal.y = -normal.y;
	normal.z = -normal.z;
	d=-d;
}

bool Plane::IntersectsRay(const Ray &ray, float *distance) const {
    float dot = glm::dot(normal, ray.direction);
    if (dot>0.0001f) return false;
    *distance = (d - glm::dot(normal, ray.position)) / dot;
    return true;
}


