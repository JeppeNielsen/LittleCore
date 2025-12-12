//
// Created by Jeppe Nielsen on 13/10/2025.
//
#pragma once
#include "GuiHelper.hpp"
#include "Texturable.hpp"
#include "ShaderResource.hpp"
#include "Mesh.hpp"
#include "FontResource.hpp"

namespace Internal {

    template<typename T>
    void DrawResourceInfo(const std::string& name, GuiHelper::DrawOptions& options, LittleCore::ResourceHandle<T>& component) {
        auto info = options.resourceManager.GetInfo(component);
        if (!info.isMissing) {
            std::filesystem::path filePath = info.path;
            ImGui::Text("%s: %s", name.c_str(), filePath.filename().c_str());
        } else {
            ImGui::Text("Missing!");
        }
    }

    template<typename T>
    void DrawResourceDragging(GuiHelper::DrawOptions& options, LittleCore::ResourceHandle<T>& component) {
        if (ImGui::BeginDragDropTarget()) {
            const ImGuiPayload* payload = ImGui::GetDragDropPayload();
            std::string path(static_cast<const char*>(payload->Data), payload->DataSize);
            auto id = options.resourceManager.pathMapper.GetGuid(path);

            if (options.resourceManager.IsTypeValid<T>(id) &&
                ImGui::AcceptDragDropPayload("ResourcePath")) {
                component = options.resourceManager.Create<T>(id);
                options.didChange = true;
            }
            ImGui::EndDragDropTarget();
        }
    }
}

template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::Texturable>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::Texturable>& component) {
    Internal::DrawResourceInfo("Texture", options, component);
    Internal::DrawResourceDragging(options, component);
}

template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::ShaderResource>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::ShaderResource>& component) {
    Internal::DrawResourceInfo("Shader", options, component);
    Internal::DrawResourceDragging(options, component);
}

template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::Mesh>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::Mesh>& component) {
    Internal::DrawResourceInfo("Mesh", options, component);
    Internal::DrawResourceDragging(options, component);
}

template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::PrefabResource>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::PrefabResource>& component) {
    Internal::DrawResourceInfo("Prefab", options, component);
    Internal::DrawResourceDragging(options, component);
}

template<>
void GuiHelper::Draw<LittleCore::ResourceHandle<LittleCore::FontResource>> (GuiHelper::DrawOptions& options, const std::string& name, LittleCore::ResourceHandle<LittleCore::FontResource>& component) {
    Internal::DrawResourceInfo("Font", options, component);
    Internal::DrawResourceDragging(options, component);
}




