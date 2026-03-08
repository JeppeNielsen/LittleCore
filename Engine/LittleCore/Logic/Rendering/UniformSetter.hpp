//
// Created by Jeppe Nielsen on 17/12/2025.
//


#pragma once
#include "SokolDirect.hpp"
#include "UniformCollection.hpp"
#include "Math.hpp"
#include <entt/entt.hpp>
#include <TupleHelper.hpp>

namespace LittleCore {

    class UniformSetterBase {

    public:
        UniformCollection* uniformCollection;

    public:
        void Set(const std::string& id, const sg_image texture, uint8_t stage) {
            (void)id;
            (void)texture;
            (void)stage;
        }

        void Set(const std::string& id, const vec4& vector) {
            (void)id;
            (void)vector;
        }

        void Set(const std::string& id, const mat3x3& matrix) {
            (void)id;
            (void)matrix;
        }

        void Set(const std::string& id, const mat4x4& matrix) {
            (void)id;
            (void)matrix;
        }

    };

    template<typename ...T>
    class UniformSetter : public UniformSetterBase {

    private:
        using ComponentTypes = std::tuple<T*...>;
        ComponentTypes componentTypes;

    public:
        void SetFromEntity(const entt::registry& registry, entt::entity entity) {
            TupleHelper::for_each(componentTypes, [&](auto* type) {
                using ComponentType = std::remove_pointer_t<decltype(type)>;

                ComponentType* componentType = registry.try_get<ComponentType>(entity);

                if (componentType) {
                    componentType->SetUniform(*this);
                }
            });
        }
    };

}
