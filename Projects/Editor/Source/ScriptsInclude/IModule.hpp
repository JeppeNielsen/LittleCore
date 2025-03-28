//
// Created by Jeppe Nielsen on 13/01/2024.
//

#pragma once
#include "EngineContext.hpp"
#include "imgui.h"

struct EditorRenderer;

struct IModule {
    virtual ~IModule() {}
    virtual void Initialize(EngineContext& engineContext) { };
    virtual void Update(float dt) = 0;
    virtual void Render(EditorRenderer* editorRenderer) = 0;

    virtual void OnGui() = 0;

    virtual void InvokeOnGui(ImGuiContext* imGuiContext) {
        ImGui::SetCurrentContext(imGuiContext);
        OnGui();
    }
};