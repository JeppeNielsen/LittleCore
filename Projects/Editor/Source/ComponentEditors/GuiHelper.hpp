//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include <string>

class GuiHelper {
public:

    static void DrawHeader(const std::string& name);

    template<typename T>
    static void Draw(const std::string& name, T& value);

};
