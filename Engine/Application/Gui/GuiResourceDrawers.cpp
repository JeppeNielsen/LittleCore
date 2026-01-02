//
// Created by Jeppe Nielsen on 29/12/2025.
//

#include "GuiResourceDrawers.hpp"

using namespace LittleCore;


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




