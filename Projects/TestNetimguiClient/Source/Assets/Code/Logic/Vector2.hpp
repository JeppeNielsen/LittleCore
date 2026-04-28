#pragma once
#include <cmath>
#include <iostream>

struct Vector2 {
    float x;
    float y;
    
    float Length() {
        return sqrtf(x*x+y*y);
    }

};


void Testing() {

    Vector2 a;
    
    std::cout << a.Length() << std::endl;

}
