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
    class EntityGuiDrawer : public EntityGuiDrawerBase, public IComponentTypeNameGetter, public IComponentGuiDrawer {
        using ComponentTypes = std::tuple<T*...>;

        ComponentTypes componentTypes;
    public:

        std::vector<std::string> GetComponentTypeNames(entt::registry& registry, entt::entity entity) override {
            std::vector<std::string> componentTypeNames;
            TupleHelper::for_each(componentTypes, [&componentTypeNames, &registry, entity](auto componentPtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(componentPtr)>;
                if (registry.all_of<ComponentType>(entity)) {
                    std::string name = TypeUtility::GetClassName<ComponentType>();
                    componentTypeNames.push_back(name);
                }
            });
            return componentTypeNames;
        }

        void DrawComponent(std::string& componentTypeName, entt::registry& registry, entt::entity entity) override {

            TupleHelper::for_each(componentTypes, [&componentTypeName, &registry, entity, this](auto componentPtr) {
                using ComponentType = typename std::remove_pointer_t<decltype(componentPtr)>;

                if (!registry.all_of<ComponentType>(entity)) {
                    return;
                }

                std::string name = TypeUtility::GetClassName<ComponentType>();
                if (componentTypeName != name) {
                    return ;
                }

                DrawComponent<ComponentType>(name, registry, entity);

            });

        }

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
            context.currentEntity = entity;
            context.currentRegistry = &registry;
            context.componentTypeGetter = this;
            context.componentGuiDrawer = this;
            bool didChange = ObjectGuiDrawer<TComponent>::Draw(context, component);
            if (didChange) {
                registry.patch<TComponent>(entity);
            }
            return didChange;
        }

        bool Draw(entt::registry& registry, entt::entity entity) override {
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

        bool DrawComponentMenu(entt::registry& registry, entt::entity entity) override {

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
