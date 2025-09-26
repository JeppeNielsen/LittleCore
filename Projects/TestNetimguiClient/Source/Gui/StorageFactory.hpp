//
// Created by Jeppe Nielsen on 25/09/2025.
//

#pragma once


template <class T>
concept BasicStruct =
std::is_class_v<T>;

struct FactoryContext {
    int kind;
    std::string name;
};

struct Factory {

    template<typename T>
    static void* Create(FactoryContext& context);

};

namespace entt {
    template<BasicStruct T>
    void* storage_factory(void* args) {
        FactoryContext* context = reinterpret_cast<FactoryContext*>(args);
        void* ptr = Factory::Create<T>(*context);
        return ptr;
    }
}

#include <entt/entt.hpp>
#include "ComponentGuiDrawer.hpp"

template<typename T>
void* Factory::Create(FactoryContext& context) {
    auto drawer = new LittleCore::ComponentGuiDrawer<T>();
    drawer->name = context.name;
    return drawer;
}