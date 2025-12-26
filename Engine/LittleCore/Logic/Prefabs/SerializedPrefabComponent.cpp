//
// Created by Jeppe Nielsen on 23/12/2025.
//

#include "SerializedPrefabComponent.hpp"
#include <glaze/glaze.hpp>
#include "SerializationContext.hpp"

template<>
struct glz::meta<LittleCore::SerializedPrefabComponent> {
    using T = LittleCore::SerializedPrefabComponent;

    static constexpr auto read = [](T& self, const std::string& guid_str, glz::context& context) {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        //if (c->resourceManager) {
        //    self = c->resourceManager->Create<TResource>(guid_str);
        //}
        self.data = "";
    };

    static constexpr auto write = [](const T& self, glz::context& context) -> std::string {
        LittleCore::SerializationContext* c = static_cast<LittleCore::SerializationContext*>(context.userData);
        return c->;
    };

    static constexpr auto value = glz::object(
            "componentId", &T::componentId,
            "data", glz::custom<read, write>
    );
};
