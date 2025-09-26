//
// Created by Jeppe Nielsen on 16/09/2025.
//


#pragma once
#include "ReflectionUtility.hpp"
#include "GuiHelper.hpp"

class ObjectGuiDrawer {
public:

    template<typename T>
    static bool Draw(T&& object) {

        GuiHelper::DrawOptions options;
        LittleCore::ReflectionUtility::IterateMembers(object, [&options](const std::string_view& name, auto& value) {
            std::string nameValue(name);
            options.recurse = false;
            ImGui::PushID(&value);
            GuiHelper::Draw(options, nameValue, value);
            ImGui::PopID();
            return options.recurse;
        });

        return options.didChange;
    }


};
