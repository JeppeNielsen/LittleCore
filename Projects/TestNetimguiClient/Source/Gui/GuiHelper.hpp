//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include <string>

class GuiHelper {
public:

    static void DrawHeader(const std::string& name);

    template<typename T>
    static void Draw(bool& didChange, const std::string& name, T& value);

    static void DrawColor(bool& didChange, const std::string& name, uint32_t& color);
    static void DrawLabel(const std::string& label);
    static bool InputText(const std::string& label, std::string& str);
};
