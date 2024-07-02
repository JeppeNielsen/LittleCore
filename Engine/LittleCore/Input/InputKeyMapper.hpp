//
// Created by Jeppe Nielsen on 09/02/2024.
//

#pragma once

#include "InputKey.hpp"

namespace LittleCore {

    class InputKeyMapper {
    public:
        static InputKey FromId(int id);
        static int ToId(InputKey id);

    };

}