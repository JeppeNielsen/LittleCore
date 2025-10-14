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
#include "EntityGuiDrawerContext.hpp"

namespace LittleCore {

    struct EntityGuiDrawerBase {

        EntityGuiDrawerBase(EntityGuiDrawerContext& context) : context(context) {}

        ~EntityGuiDrawerBase() = default;
        virtual bool Draw(entt::registry& registry, entt::entity entity) = 0;
        virtual bool DrawComponentMenu(entt::registry& registry, entt::entity entity) = 0;

        EntityGuiDrawerContext& context;
    };

    template <typename ...T>
    class EntityGuiDrawer : public EntityGuiDrawerBase {
        using ComponentTypes = std::tuple<T*...>;

        ComponentTypes componentTypes;
    public:

        EntityGuiDrawer(EntityGuiDrawerContext& context) : EntityGuiDrawerBase(context) {}

        template<typename TComponent>
        bool DrawComponent(const std::string& name, entt::registry& registry, entt::entity entity)  {
            GuiHelper::DrawHeader(name);

            ImVec2 gameViewMin = ImGui::GetItemRectMin();
            ImVec2 gameViewMax = ImGui::GetItemRectMax();

            ImVec2 canvasSize = ImVec2(gameViewMax.x - gameViewMin.x, gameViewMax.y - gameViewMin.y);
            ImGui::SetCursorScreenPos(gameViewMin);
            ImGui::InvisibleButton(name.c_str(), canvasSize, ImGuiButtonFlags_MouseButtonRight);

            std::string ctxStr = name + "_ctx";
            if (ImGui::BeginPopupContextItem(ctxStr.c_str())) {
                if (ImGui::MenuItem("Remove")) {
                    registry.remove<TComponent>(entity);
                }
                ImGui::EndPopup();
            }

            if (!registry.all_of<TComponent>(entity)) {
                return false;
            }

            TComponent& component = registry.get<TComponent>(entity);
            bool didChange = ObjectGuiDrawer::Draw(context, component);
            if (didChange) {
                registry.patch<TComponent>(entity);
            }
            return didChange;
        }

        bool Draw(entt::registry& registry, entt::entity entity) {
            bool didChange = false;
            TupleHelper::for_each(componentTypes, [&didChange, &registry, entity, this](auto componentPtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(componentPtr)>;

                if (!registry.all_of<ComponentType>(entity)) {
                    return;
                }

                std::string name = TypeUtility::GetClassName<ComponentType>();
                didChange |= DrawComponent<ComponentType>(name, registry, entity);
            });

            return didChange;
        }

        bool DrawComponentMenu(entt::registry& registry, entt::entity entity) {

            bool didChange = false;
            if (ImGui::BeginPopupContextWindow(NULL, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) {

                TupleHelper::for_each(componentTypes, [&didChange, &registry, entity](auto componentPtr) {
                    using ComponentType = typename std::remove_pointer_t<decltype(componentPtr)>;

                    if (registry.all_of<ComponentType>(entity)) {
                        return;
                    }

                    std::string name = TypeUtility::GetClassName<ComponentType>();
                    std::string itemText = "Add "  + name;

                    if (ImGui::MenuItem(itemText.c_str())) {
                        registry.emplace<ComponentType>(entity);
                        didChange = true;
                    }
                });

                ImGui::EndPopup();
            }

            return didChange;
        }

    };
}
