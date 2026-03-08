//
// Created by Jeppe Nielsen on 18/01/2025.
//


#pragma once
#include "SokolDirect.hpp"
#include "ResourceHandle.hpp"
#include "ResourceComponent.hpp"

namespace LittleCore {
    struct Texturable : public ResourceComponent<Texturable> {
        sg_image texture = {SG_INVALID_ID};
    };
}
