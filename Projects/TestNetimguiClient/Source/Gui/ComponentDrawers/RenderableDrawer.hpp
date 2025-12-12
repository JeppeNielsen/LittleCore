//
// Created by Jeppe Nielsen on 04/11/2025.
//

#pragma once
#include "Renderable.hpp"
#include "ObjectGuiDrawer.hpp"
#include "GuiResourceDrawers.hpp"

namespace LittleCore {
    template<>
    bool ObjectGuiDrawer<Renderable>::Draw(EntityGuiDrawerContext& context,
                                           Renderable& object) {

        GuiHelper::DrawOptions drawOptions(context.resourceManager, false, false);
        GuiHelper::Draw(drawOptions, "Shader", object.shader);


        static const char* BlendModeNames[] = {
                "Off", "Alpha", "Add", "Multiply"
        };

        int current = static_cast<int>(object.blendMode);

        if (ImGui::BeginCombo("BlendMode", BlendModeNames[current]))  // preview
        {
            for (int i = 0; i < IM_ARRAYSIZE(BlendModeNames); ++i)
            {
                bool is_selected = (current == i);
                if (ImGui::Selectable(BlendModeNames[i], is_selected))
                {
                    current = i;
                    object.blendMode = static_cast<BlendMode>(i);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus(); // nice UX
            }
            ImGui::EndCombo();
        }


        return drawOptions.didChange;
    };

}