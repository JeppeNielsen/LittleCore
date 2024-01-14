//
// Created by Jeppe Nielsen on 14/01/2024.
//

#pragma once
#include <functional>
#include "IWindow.hpp"

class EngineContext {
public:
    std::function<IWindow*()> CreateWindow;

};
