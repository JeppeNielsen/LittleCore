//
// Created by Jeppe Nielsen on 07/01/2026.
//


#pragma once
#include "Project.hpp"
#include "FileTreeNode.hpp"
#include "PrefabContext.hpp"

using namespace LittleCore;

class PrefabWindow {
public:

    enum class PrefabEvent {
        NewFile,
    };

    using Callback = std::function<void(const FileTreeNode& node, PrefabEvent event)>;
    PrefabWindow(const Project& project, PrefabContext& context, Callback callback);
    void DrawUI();
private:
    void DrawNode(const FileTreeNode& node);
    const Project& project;
    PrefabContext& context;
    Callback callback;
};
