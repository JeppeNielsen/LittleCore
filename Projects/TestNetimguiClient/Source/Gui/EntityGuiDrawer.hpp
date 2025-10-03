//
// Created by Jeppe Nielsen on 19/09/2025.
//


#pragma once
#include <entt/entt.hpp>
#include <tuple>
#include "TupleHelper.hpp"
#include "GuiHelper.hpp"
#include "ObjectGuiDrawer.hpp"
#include "TypeUtility.hpp"

namespace LittleCore {

    template <typename ...T>
    class EntityGuiDrawer {
        using ComponentTypes = std::tuple<T*...>;

        ComponentTypes componentTypes;
    public:

        template<typename TComponent>
        static bool DrawComponent(const std::string& name, entt::registry& registry, entt::entity entity)  {
            GuiHelper::DrawHeader(name);
            TComponent& component = registry.get<TComponent>(entity);
            bool didChange = ObjectGuiDrawer::Draw(component);
            if (didChange) {
                registry.patch<TComponent>(entity);
            }
            return didChange;
        }

        bool Draw(entt::registry& registry, entt::entity entity) {
            bool didChange = false;
            TupleHelper::for_each(componentTypes, [&didChange, &registry, entity](auto componentPtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(componentPtr)>;

                if (!registry.all_of<ComponentType>(entity)) {
                    return;
                }

                std::string name = TypeUtility::GetClassName<ComponentType>();
                didChange |= DrawComponent<ComponentType>(name, registry, entity);
            });

            return didChange;
        }

    };
}
