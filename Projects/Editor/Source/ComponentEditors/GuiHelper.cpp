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

template<>
void GuiHelper::Draw<bool>(bool& didChange, const std::string &name, bool& value) {
    ImGui::Checkbox(name.c_str(), &value);
    didChange |= ImGui::IsItemEdited();
}

template<>
void GuiHelper::Draw<float>(bool& didChange, const std::string &name, float &value) {
    ImGui::InputFloat(name.c_str(), &value);
    didChange |= ImGui::IsItemEdited();
}

template<>
void GuiHelper::Draw<vec2>(bool& didChange, const std::string &name, vec2 &value) {
    ImGui::InputFloat2(name.c_str(), &value.x);
    didChange |= ImGui::IsItemEdited();
}

template<>
void GuiHelper::Draw<vec3>(bool& didChange, const std::string &name, vec3 &value) {
    ImGui::InputFloat3(name.c_str(), &value.x);
    didChange |= ImGui::IsItemEdited();
}

template<>
void GuiHelper::Draw<quat>(bool& didChange, const std::string &name, quat &value) {
    vec3 euler = glm::eulerAngles(value);
    vec3 degrees = glm::degrees(euler);
    ImGui::InputFloat3(name.c_str(), &degrees.x);
    vec3 radians = glm::radians(degrees);
    value = glm::quat(radians);

    didChange |= ImGui::IsItemEdited();
}

template<>
void GuiHelper::Draw<Rect>(bool& didChange, const std::string& name, Rect& value) {
    Draw(didChange, "min", value.min);
    Draw(didChange, "max", value.max);
}

