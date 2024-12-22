//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "GuiHelper.hpp"
#include "Math.hpp"
#include <imgui.h>

void GuiHelper::DrawHeader(const std::string &name) {
    ImGui::SeparatorText(name.c_str());
}

template<>
void GuiHelper::Draw<vec3>(const std::string &name, vec3 &value) {
    ImGui::InputFloat3(name.c_str(), &value.x);
}

template<>
void GuiHelper::Draw<quat>(const std::string &name, quat &value) {
    vec3 euler = glm::eulerAngles(value);
    vec3 degrees = glm::degrees(euler);
    ImGui::InputFloat3(name.c_str(), &degrees.x);
    vec3 radians = glm::radians(degrees);
    value = glm::quat(radians);
}

