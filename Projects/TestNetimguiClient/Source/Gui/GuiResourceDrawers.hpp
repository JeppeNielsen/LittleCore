//
// Created by Jeppe Nielsen on 13/10/2025.
//
#pragma once
#include "GuiHelper.hpp"
#include "TextureResource.hpp"
#include "ShaderResource.hpp"


template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::TextureResource>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::TextureResource>& component) {

    auto info = options.resourceManager.GetInfo(component);

    if (!info.isMissing) {
        std::filesystem::path filePath = info.path;
        ImGui::Text("Texture: %s", filePath.filename().c_str());
    } else {
        ImGui::Text("Missing!");
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourcePath")) {
            std::string path(static_cast<const char*>(payload->Data), payload->DataSize);
            auto id = options.resourceManager.pathMapper.GetGuid(path);
            component = options.resourceManager.Create<LittleCore::TextureResource>(id);
        }
        ImGui::EndDragDropTarget();
    }

}


template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::ShaderResource>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::ShaderResource>& component) {

    auto info = options.resourceManager.GetInfo(component);

    if (!info.isMissing) {
        std::filesystem::path filePath = info.path;
        ImGui::Text("Shader: %s", filePath.filename().c_str());
    } else {
        ImGui::Text("Missing!");
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ResourcePath")) {
            std::string path(static_cast<const char*>(payload->Data), payload->DataSize);
            auto id = options.resourceManager.pathMapper.GetGuid(path);
            component = options.resourceManager.Create<LittleCore::ShaderResource>(id);
        }
        ImGui::EndDragDropTarget();
    }

}




