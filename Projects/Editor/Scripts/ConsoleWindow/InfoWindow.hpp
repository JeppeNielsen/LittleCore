//
// Created by Jeppe Nielsen on 13/07/2024.
//

#pragma once
#include <iostream>
#include "imgui_stdlib.h"

void ShowInfoWindow(int currentFps, int frameCounter) {

    ImGui::Begin("Info Window");

    ImGui::Text("This is the info window!");


    std::stringstream s;
    s << currentFps;

    std::string t = "Fps: " + s.str();
    auto* ttt = t.c_str();

    ImGui::Text("%s", t.c_str());

    for (int i = 0; i < 10; ++i) {
        if (ImGui::Button(std::to_string(i).c_str())) {
            std::cout << "Button #"<<i<<" clicked \n";
        }
    }

    std::string frameInfo = "Frame: " + std::to_string(frameCounter);
    ImGui::Text("%s", frameInfo.c_str());

    ImGui::End();

}