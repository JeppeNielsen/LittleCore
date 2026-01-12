//
// Created by Jeppe Nielsen on 09/01/2026.
//


#pragma once
#include <string>

struct PrefabSettings {
    bool autoSave = true;
};

struct PrefabContext {
    PrefabSettings settings;
    std::string selectedPath;
    std::string previousSelectedPath;
    bool isDirty = false;
};
