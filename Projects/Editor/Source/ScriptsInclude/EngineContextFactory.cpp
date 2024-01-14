//
// Created by Jeppe Nielsen on 14/01/2024.
//

#include "EngineContextFactory.hpp"
#include <iostream>

EngineContext EngineContextFactory::Create() {

    EngineContext context;
    context.CreateWindow = [] () {
        std::cout << "created window \n";
        return new IWindow();
    };

    return context;
}
