//
// Created by Jeppe Nielsen on 03/11/2025.
//

#pragma once
#include "PrefabExposedComponents.hpp"
#include "ObjectGuiDrawer.hpp"

namespace LittleCore {
    template<>
    bool ObjectGuiDrawer<PrefabExposedComponents>::Draw(EntityGuiDrawerContext& context,
                                                        PrefabExposedComponents& object) {

        auto componentTypeNames = context.componentTypeGetter->GetComponentTypeNames(*context.currentRegistry, context.currentEntity);

        for(auto name : componentTypeNames ) {
            ImGui::PushID(name.c_str());
            auto found = std::find(object.exposedComponents.begin(), object.exposedComponents.end(), name);
            bool wasChecked = found != object.exposedComponents.end();
            bool changed = ImGui::Checkbox(name.c_str(), &wasChecked);

            if (changed) {
                if (!wasChecked) {
                    object.exposedComponents.erase(found);
                } else {
                    object.exposedComponents.push_back(name);
                }
            }
            ImGui::PopID();
        }

        std::erase_if(object.exposedComponents, [&](const std::string& c) {
            return std::find(componentTypeNames.begin(), componentTypeNames.end(), c) == componentTypeNames.end();
        });

        return true;
    };

}