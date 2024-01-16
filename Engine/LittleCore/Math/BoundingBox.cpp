#include "BoundingBox.hpp"
#include <math.h>

using namespace LittleCore;

BoundingBox::BoundingBox() {
}

BoundingBox::~BoundingBox()
{
}

BoundingBox::BoundingBox(const BoundingBox& other) {
    center = other.center;
    extends = other.extends;
}

BoundingBox::BoundingBox(const vec3& center, const vec3& extends) {
    this->center = center;
    this->extends = extends;
}

bool BoundingBox::Intersects(const BoundingBox& other) const {
    const vec3 v = other.center - center;
    if (std::fabs(v.x)>(extends.x + other.extends.x)) return false;
    if (std::fabs(v.y)>(extends.y + other.extends.y)) return false;
    if (std::fabs(v.z)>(extends.z + other.extends.z)) return false;
    return true;
}

float BoundingBox::Volume() const {
    return extends.x * extends.y * extends.z;
}

bool BoundingBox::operator ==(const BoundingBox &other) {
    if (extends!=other.extends) return false;
    if (center!=other.center) return false;
    return true;
}

bool BoundingBox::operator !=(const BoundingBox &other) {
    if (extends!=other.extends) return true;
    if (center!=other.center) return true;
    return false;
}

bool BoundingBox::Contains(const BoundingBox& other) const {
    
    vec3 min1 = center - extends * 0.5f;
    vec3 max1 = center + extends * 0.5f;

    vec3 min2 = other.center - other.extends * 0.5f;
    vec3 max2 = other.center + other.extends * 0.5f;

    if (min2.x<min1.x) return false;
    if (max2.x>max1.x) return false;

    if (min2.y<min1.y) return false;
    if (max2.y>max1.y) return false;

    if (min2.z<min1.z) return false;
    if (max2.z>max1.z) return false;

    return true;
};

void BoundingBox::CreateWorldAligned(const mat4x4& matrix, BoundingBox& boundingBox) const {
    vec3 halfExtends = extends * 0.5f;

    boundingBox.center = matrix * vec4(center, 1.0f); //matrix.TransformPosition(center);
    
    //float fInvW = 1.0f / ( fabsf(matrix.m[3][0]) * halfExtends.x + fabsf(matrix.m[3][1]) * halfExtends.y + fabsf(matrix.m[3][2]) * halfExtends.z + fabsf(matrix.m[3][3]) );
    
    vec3 extends;

    extends.x = (fabsf(matrix[0][0]) * halfExtends.x + fabsf(matrix[1][0]) * halfExtends.y + fabsf(matrix[2][0]) * halfExtends.z);// * fInvW;
    extends.y = (fabsf(matrix[0][1]) * halfExtends.x + fabsf(matrix[1][1]) * halfExtends.y + fabsf(matrix[2][1]) * halfExtends.z);// * fInvW;
    extends.z = (fabsf(matrix[0][2]) * halfExtends.x + fabsf(matrix[1][2]) * halfExtends.y + fabsf(matrix[2][2]) * halfExtends.z);// * fInvW;

    boundingBox.extends = extends * 2.0f;
}
