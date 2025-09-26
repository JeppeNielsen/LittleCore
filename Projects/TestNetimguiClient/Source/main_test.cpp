//
// Created by Jeppe Nielsen on 21/09/2025.
//

#include <concepts>
#include <iostream>

template <class T>
concept BasicStruct =
std::is_class_v<T>;

struct FactoryContext {
    int kind;
};


namespace entt {
    template<BasicStruct T>
    void* storage_factory(void* args) {
        FactoryContext* context = reinterpret_cast<FactoryContext*>(args);

        std::cout << "Factory Basic<T> called :" <<context->kind<<""<< "\n";
        return nullptr;
    }
}



#include <entt/entt.hpp>


struct Position {
    float x;
    float y;
};

struct Velocity {
    float vx;
    float vy;
};

/*template <class T>
concept BasicStruct =
std::is_class_v<T> &&
std::is_aggregate_v<T> &&            // brace-initializable aggregate
std::is_standard_layout_v<T> &&      // C-like layout rules
std::is_trivially_copyable_v<T> &&   // memcpy-safe
!std::is_polymorphic_v<T>;
*/


int main_old() {

    entt::registry registry;

    auto e1 = registry.create();
    registry.emplace<Position>(e1);
    registry.emplace<Velocity>(e1);


    FactoryContext context;
    context.kind = 23;
    for(auto storage: registry.storage()) {
        if(storage.second.contains(e1)) {
            auto type = storage.second.type();
            std::cout << "Entity has component: " << type.name() << "\n";
            auto ptr = storage.second.CreateFactory(&context);

            std::cout << "ptr : "<<ptr<<"\n";
        }
    }

    return 0;
}