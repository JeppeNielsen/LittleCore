//
// Created by Jeppe Nielsen on 22/12/2024.
//


#pragma once
#include <string>
#include <type_traits>
#include <concepts>
#include "Math.hpp"
#include <imgui.h>
#include <iostream>
#include "Rect.hpp"
#include "ResourceHandle.hpp"
#include "Texturable.hpp"
#include "DefaultResourceManager.hpp"
#include "Color.hpp"
#include "BlendMode.hpp"

class GuiHelper {
public:

    struct DrawOptions {
        LittleCore::DefaultResourceManager& resourceManager;
        bool didChange = false;
        bool recurse = false;
    };

    template<typename T>
    static void Draw(DrawOptions& options, const std::string& name, T& value) {
        options.recurse = true;
    }

    template<>
    void Draw<bool>(DrawOptions& options, const std::string &name, bool& value) {
        ImGui::Checkbox(name.c_str(), &value);
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<float>(DrawOptions& options, const std::string &name, float &value) {
        ImGui::InputFloat(name.c_str(), &value,0.0f,0.0f, "%.4f");
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<vec2>(DrawOptions& options, const std::string &name, vec2 &value) {
        ImGui::InputFloat2(name.c_str(), &value.x, "%.4f");
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<vec3>(DrawOptions& options, const std::string &name, vec3 &value) {
        ImGui::InputFloat3(name.c_str(), &value.x, "%.4f");
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<LittleCore::Color>(DrawOptions& options, const std::string &name, LittleCore::Color &value) {
        ImGui::ColorEdit4(name.c_str(), &value.x, ImGuiColorEditFlags_DefaultOptions_ | ImGuiColorEditFlags_AlphaBar );
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<quat>(DrawOptions& options, const std::string &name, quat &value) {
        vec3 euler = glm::eulerAngles(value);
        vec3 degrees = glm::degrees(euler);
        ImGui::InputFloat3(name.c_str(), &degrees.x);
        vec3 radians = glm::radians(degrees);
        value = glm::quat(radians);

        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<LittleCore::Rect>(DrawOptions& options, const std::string& name, LittleCore::Rect& value) {
        Draw(options, "min", value.min);
        Draw(options, "max", value.max);
    }

    template<>
    void Draw<std::string>(DrawOptions& options, const std::string& name, std::string& value) {
        //ImGui::Text(value.c_str(), value.c_str());
        options.didChange |= GuiHelper::InputText(name, value);
    }

    template<>
    void Draw<int>(DrawOptions& options, const std::string &name, int& value) {
        ImGui::InputInt(name.c_str(), &value);
        options.didChange |= ImGui::IsItemEdited();
    }

    template<>
    void Draw<LittleCore::BlendMode>(DrawOptions& options, const std::string& name, LittleCore::BlendMode& value) {

        static const char* BlendModeNames[] = {
                "Off", "Alpha", "Add", "Multiply"
        };

        int current = static_cast<int>(value);

        if (ImGui::BeginCombo("BlendMode", BlendModeNames[current]))  // preview
        {
            for (int i = 0; i < IM_ARRAYSIZE(BlendModeNames); ++i)
            {
                bool is_selected = (current == i);
                if (ImGui::Selectable(BlendModeNames[i], is_selected))
                {
                    current = i;
                    value = static_cast<LittleCore::BlendMode>(i);
                    options.didChange = true;
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // nice UX
            }
            ImGui::EndCombo();
        }
    }

    static void DrawHeader(const std::string& name);
    static void DrawColor(bool& didChange, const std::string& name, uint32_t& color);
    static void DrawLabel(const std::string& label);
    static bool InputText(const std::string& label, std::string& str);
};
