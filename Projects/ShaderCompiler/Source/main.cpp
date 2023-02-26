
#include <iostream>
#include "shaderc.h"

int main() {

    bgfx::Options options;
   

    bgfx::compileMetalShader(options, 0, "code", nullptr);


    return 0;
}
