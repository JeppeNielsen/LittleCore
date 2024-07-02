//
// Created by Jeppe Nielsen on 09/02/2024.
//

#include "InputKeyMapper.hpp"

using namespace LittleCore;

InputKey InputKeyMapper::FromId(int id) {
    return (InputKey)id;
}

int InputKeyMapper::ToId(InputKey id) {
    return (int)id;
}

