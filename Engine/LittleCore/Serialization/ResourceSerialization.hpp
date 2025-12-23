//
// Created by Jeppe Nielsen on 22/12/2025.
//

#pragma once
#include <glaze/glaze.hpp>
#include "ResourceHandle.hpp"
#include "DefaultResourceManager.hpp"
#include "RegistrySerializer.hpp"
#include "SerializationContext.hpp"

template<typename TResource>
struct glz::meta<LittleCore::ResourceHandle<TResource>> {
    using T = LittleCore::ResourceHandle<TResource>;

    static constexpr auto read_ref = [](T& self, const std::string& guid_str, glz::context& context) {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        if (c->resourceManager) {
            self = c->resourceManager->Create<TResource>(guid_str);
        }
    };

    static constexpr auto write_ref = [](const T& self, glz::context& context) {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        if (c->resourceManager) {
            auto info = c->resourceManager->GetInfo(self);
            return info.id;
        }
        return std::string();
    };

    static constexpr auto value = glz::object(
            "id", glz::custom<read_ref, write_ref>
    );
};

template <class T>
concept DerivedFromResourceComponent =
std::is_base_of_v<LittleCore::ResourceComponent<T>, T>;

template<typename TResourceComponent>
requires DerivedFromResourceComponent<TResourceComponent>
struct glz::meta<TResourceComponent> {
    using T = TResourceComponent;

    static constexpr auto read_ref = [](T& self, const std::string& guid_str, glz::context& context) {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        if (c->resourceManager) {
            self.handle = c->resourceManager->Create<typename T::ContainedType>(guid_str);
        }
    };

    static constexpr auto write_ref = [](const T& self, glz::context& context) {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        if (c->resourceManager) {
            auto info = c->resourceManager->GetInfo(self.handle);
            return info.id;
        }
        return std::string();
    };

    static constexpr auto value = glz::object(
            "id", glz::custom<read_ref, write_ref>
    );
};
