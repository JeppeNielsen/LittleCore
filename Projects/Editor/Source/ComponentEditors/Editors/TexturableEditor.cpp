//
// Created by Jeppe Nielsen on 22/12/2024.
//

#include "TexturableEditor.hpp"
#include "../GuiHelper.hpp"
#include <imgui.h>
#include <iostream>
#include <filesystem>

bool TexturableEditor::Draw(entt::registry &registry, entt::entity entity, LittleCore::Texturable& component) {
    bool changed = false;
    GuiHelper::DrawHeader("Texturable");

    auto info = resourceManager->GetInfo(component.handle);

    if (!info.isMissing) {
        std::filesystem::path filePath = info.path;
        ImGui::Text("Texture: %s", filePath.filename().c_str());
    } else {
        ImGui::Text("Missing!");
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourcePath")) {
            std::string path(static_cast<const char*>(payload->Data), payload->DataSize);
            auto id = resourceManager->pathMapper.GetGuid(path);
            component.handle = resourceManager->Create<LittleCore::Texturable>(id);
        }
        ImGui::EndDragDropTarget();
    }


    return changed;
}

void TexturableEditor::SetPathMapper(LittleCore::DefaultResourceManager& resourceManager) {
    this->resourceManager = &resourceManager;
}
