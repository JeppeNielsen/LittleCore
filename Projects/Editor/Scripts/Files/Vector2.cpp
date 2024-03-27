//
// Created by Jeppe Nielsen on 27/03/2024.
//

#include "Vector2.hpp"
#include <cmath>

float Vector2::Length() {
    return sqrt(x*x+y*y);
}