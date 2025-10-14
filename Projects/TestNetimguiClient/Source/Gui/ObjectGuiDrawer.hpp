//
// Created by Jeppe Nielsen on 16/09/2025.
//


#pragma once
#include "ReflectionUtility.hpp"
#include "GuiHelper.hpp"
#include "Texturable.hpp"
#include "EntityGuiDrawerContext.hpp"

namespace LittleCore {
    class ObjectGuiDrawer {
    public:

        template<typename T>
        static bool Draw(EntityGuiDrawerContext& context, T&& object) {

            GuiHelper::DrawOptions options(context.resourceManager);
            LittleCore::ReflectionUtility::IterateMembers(object,
                                                          [&options](const std::string_view& name, auto& value) {
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

}
