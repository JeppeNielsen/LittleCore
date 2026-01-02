//
// Created by Jeppe Nielsen on 29/12/2025.
//

#pragma once
#include "Prefab.hpp"
#include "ObjectGuiDrawer.hpp"

template<>
bool LittleCore::ObjectGuiDrawer<LittleCore::Prefab>::Draw(EntityGuiDrawerContext& context,
                                                           Prefab& object);

