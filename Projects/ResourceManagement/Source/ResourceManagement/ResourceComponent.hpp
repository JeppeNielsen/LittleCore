//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include "ResourceHandle.hpp"

namespace LittleCore {
    template<typename T>
    struct ResourceComponent {
        ResourceHandle <T> handle;

        T *operator->() const {
            T *obj = handle.operator->();
            return obj ? obj : (T *) this;
        }
    };
}