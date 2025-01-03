//
// Created by Jeppe Nielsen on 14/01/2024.
//

#include "EngineContextFactory.hpp"
#include <iostream>

EngineContext EngineContextFactory::Create() {

    EngineContext context;
    context.imGuiContext = imGuiContext;

    return context;
}

EngineContextFactory::EngineContextFactory(ImGuiContext *imGuiContext) : imGuiContext(imGuiContext) {

}
