//
// Created by Jeppe Nielsen on 17/12/2025.
//


#pragma once
#include <bgfx/bgfx.h>
#include "UniformCollection.hpp"
#include "Math.hpp"
#include <entt/entt.hpp>
#include <TupleHelper.hpp>

namespace LittleCore {

    class UniformSetterBase {

    public:
        UniformCollection* uniformCollection;

    public:
        void Set(const std::string& id, const bgfx::TextureHandle texture, uint8_t stage) {
           auto uniform = uniformCollection->GetHandle(id, bgfx::UniformType::Enum::Sampler);
           bgfx::setTexture(stage, uniform, texture);
        }

        void Set(const std::string& id, const vec4& vector) {
            auto uniform = uniformCollection->GetHandle(id, bgfx::UniformType::Enum::Vec4);
            bgfx::setUniform(uniform, glm::value_ptr(vector));
        }

        void Set(const std::string& id, const mat3x3& matrix) {
            auto uniform = uniformCollection->GetHandle(id, bgfx::UniformType::Enum::Mat3);
            bgfx::setUniform(uniform, glm::value_ptr(matrix));
        }

        void Set(const std::string& id, const mat4x4& matrix) {
            auto uniform = uniformCollection->GetHandle(id, bgfx::UniformType::Enum::Mat4);
            bgfx::setUniform(uniform, glm::value_ptr(matrix));
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