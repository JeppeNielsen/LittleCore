//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "GuiHelper.hpp"
#include "Math.hpp"
#include <imgui.h>
#include <iostream>
#include "Rect.hpp"

using namespace LittleCore;

ImVec4 Uint32ToImVec4(uint32_t color) {
    return ImVec4(
            (float)((color >> 24) & 0xFF) / 255.0f, // R
            (float)((color >> 16) & 0xFF) / 255.0f, // G
            (float)((color >> 8) & 0xFF)  / 255.0f, // B
            (float)(color & 0xFF)          / 255.0f  // A
    );
}

uint32_t ImVec4ToUint32(const ImVec4& color) {
    return (uint32_t(color.x * 255.0f) << 24) | // R
           (uint32_t(color.y * 255.0f) << 16) | // G
           (uint32_t(color.z * 255.0f) << 8)  | // B
           (uint32_t(color.w * 255.0f));       // A
}

void GuiHelper::DrawHeader(const std::string &name) {
    ImGui::SeparatorText(name.c_str());
}

void GuiHelper::DrawColor(bool& didChange, const std::string& name, uint32_t& color) {
    ImVec4 colorVector = Uint32ToImVec4(color);
    ImGui::ColorEdit4(name.c_str(), &colorVector.x);
    color = ImVec4ToUint32(colorVector);
    didChange |= ImGui::IsItemEdited();
}



void GuiHelper::DrawLabel(const std::string& label) {
    ImGui::LabelText(label.c_str(), "");
}


static int InputTextCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
        auto* str = reinterpret_cast<std::string*>(data->UserData);
        str->resize(data->BufTextLen);
        data->Buf = str->data();
    }
    return 0;
}

bool GuiHelper::InputText(const std::string& label, std::string& str) {
    ImGuiInputTextFlags_ flags = ImGuiInputTextFlags_CallbackResize;
    return ImGui::InputText(
            label.c_str(),
            str.data(),
            str.capacity() + 1,
            flags,
            InputTextCallback,
            (void*)&str
    );
}
