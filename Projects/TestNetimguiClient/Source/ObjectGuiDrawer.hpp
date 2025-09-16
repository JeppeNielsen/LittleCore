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

        bool didChange = false;
        LittleCore::ReflectionUtility::IterateMembers(object, [&didChange](const std::string_view& name, auto& value) {
            std::string nameValue(name);
            GuiHelper::Draw(didChange, nameValue, value);
        });

        return didChange;
    }


};
