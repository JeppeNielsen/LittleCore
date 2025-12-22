//
// Created by Jeppe Nielsen on 18/12/2025.
//

#include "RenderableUniforms.hpp"
#include <iostream>
#include "Timer.hpp"
#include <glaze/glaze.hpp>
#include "ResourceHandle.hpp"
#include "Texturable.hpp"
#include "DefaultResourceManager.hpp"
#include "RegistrySerializer.hpp"
#include <entt/entt.hpp>

using namespace LittleCore;

struct RenderableObject {
    ResourceHandle<FontResource> font;
    ResourceHandle<FontResource> largeFont;
    ResourceHandle<Mesh> mesh;
};

/*
template<>
struct glz::meta<ResourceHandle<FontResource>> {
    using T = ResourceHandle<FontResource>;

    static DefaultResourceManager* resourceManager;

    static constexpr auto read_ref = [](T& self, const std::string& guid_str) {
        self = resourceManager->Create<FontResource>(guid_str);
    };

    static constexpr auto write_ref = [](const T& self) {
        auto info = resourceManager->GetInfo(self);
        return info.id;
    };

    static constexpr auto value = glz::object(
        "id", glz::custom<read_ref, write_ref>
    );
};

DefaultResourceManager* glz::meta<ResourceHandle<FontResource>>::resourceManager = nullptr;
*/

struct SerializationResources {
    static DefaultResourceManager* resourceManager;
};

DefaultResourceManager* SerializationResources::resourceManager = nullptr;

template<typename TResource>
struct glz::meta<ResourceHandle<TResource>> {
    using T = ResourceHandle<TResource>;

    static constexpr auto read_ref = [](T& self, const std::string& guid_str) {
        self = SerializationResources::resourceManager->Create<TResource>(guid_str);
    };

    static constexpr auto write_ref = [](const T& self) {
        auto info = SerializationResources::resourceManager->GetInfo(self);
        return info.id;
    };

    static constexpr auto value = glz::object(
            "id", glz::custom<read_ref, write_ref>
    );
};

template <class T>
concept DerivedFromResourceComponent =
std::is_base_of_v<ResourceComponent<T>, T>;

template<typename TResourceComponent>
requires DerivedFromResourceComponent<TResourceComponent>
struct glz::meta<TResourceComponent> {
    using T = TResourceComponent;

    static constexpr auto read_ref = [](T& self, const std::string& guid_str) {
        self.handle = SerializationResources::resourceManager->Create<typename T::ContainedType>(guid_str);
    };

    static constexpr auto write_ref = [](const T& self) {
        auto info = SerializationResources::resourceManager->GetInfo(self.handle);
        return info.id;
    };

    static constexpr auto value = glz::object(
            "id", glz::custom<read_ref, write_ref>
    );
};

int main() {

    ResourcePathMapper resourcePathMapper;

    std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/";
    resourcePathMapper.RefreshFromRootPath(path);

    DefaultResourceManager resourceManager(resourcePathMapper);

    SerializationResources::resourceManager = &resourceManager;

    resourceManager.CreateLoaderFactory<FontResourceLoaderFactory>();
    resourceManager.CreateLoaderFactory<MeshResourceLoaderFactory>();

    entt::registry registry;
    auto entity = registry.create();
    registry.emplace<Mesh>(entity).handle = resourceManager.Create<Mesh>("DD386498609F413F8ED308A6189B64B2");


    RegistrySerializer<Mesh> registrySerializer;

    auto jsonString = registrySerializer.Serialize(registry);

    std::cout<<jsonString<<"\n";

    /*
    RenderableObject test;
    test.font = resourceManager.Create<FontResource>("FD7BDACBFAE94552B2D2DB3F1051BECB");
    test.largeFont = resourceManager.Create<FontResource>("49AF1D1AC4694F3B8915863E133E0C35");

    std::string pretty_json;
    glz::write<glz::opts{.prettify = true}>(test, pretty_json);
    std::cout << pretty_json << "\n";

    RenderableObject test2;

    glz::read<glz::opts{.prettify = true}>(test2, pretty_json);


    std::cout << std::format("font equal: {0}\n", &test2.font->atlas == &test.font->atlas);
    std::cout << std::format("largeFont equal: {0}", &test2.largeFont->atlas == &test.largeFont->atlas);
*/

}


int main_old() {

    ResourcePathMapper resourcePathMapper;

    std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/";
    resourcePathMapper.RefreshFromRootPath(path);

    DefaultResourceManager resourceManager(resourcePathMapper);

    resourceManager.CreateLoaderFactory<FontResourceLoaderFactory>();
    resourceManager.CreateLoaderFactory<MeshResourceLoaderFactory>();

    SerializationResources::resourceManager = &resourceManager;

    RenderableObject test;
    test.font = resourceManager.Create<FontResource>("FD7BDACBFAE94552B2D2DB3F1051BECB");
    test.largeFont = resourceManager.Create<FontResource>("49AF1D1AC4694F3B8915863E133E0C35");

    std::string pretty_json;
    glz::write<glz::opts{.prettify = true}>(test, pretty_json);
    std::cout << pretty_json << "\n";

    RenderableObject test2;

    glz::read<glz::opts{.prettify = true}>(test2, pretty_json);


    std::cout << std::format("font equal: {0}\n", &test2.font->atlas == &test.font->atlas);
    std::cout << std::format("largeFont equal: {0}", &test2.largeFont->atlas == &test.largeFont->atlas);

    return 0;
}

int bla() {
    RenderableUniforms uniforms;


    RenderableUniforms uniforms2;



    bgfx::TextureHandle texture1 = {4};
    bgfx::TextureHandle texture2 = {8};
    uniforms.Set("texture", texture1);
    uniforms.Set("texture", texture2);

    uniforms.Set("color", vec4(1,1,1,0.5f));

    uniforms2 = uniforms;

    auto size = sizeof(RenderableUniforms);

    for(const auto& e : uniforms.GetUniforms()) {
        if (e.kind == LittleCore::RenderableUniforms::UniformEntry::Kind::Texture) {
            std::cout << "Texture : " << e.id << " = " << e.value.tex.idx << "\n";
        }

        if (e.kind == LittleCore::RenderableUniforms::UniformEntry::Kind::Vec4) {
            std::cout << "Vec4 : " << e.id <<" "<< std::format("x:{0}, y:{1}, z: {2}, w: {3}", e.value.v4.x, e.value.v4.y, e.value.v4.z, e.value.v4.w) << "\n";
        }
    }

    std::cout << "size : " << size;

    auto hash1 = uniforms.CalculateHash();
    auto hash2 = uniforms2.CalculateHash();

    std::cout << std::format("hash1 {0}, hash2 {1}", hash1, hash2)<<"\n";

    std::cout << std::format("Is the same {0}", hash1 == hash2)<<"\n";

    u_int64_t allHash = 0;

    int count = 10000000;
    Timer timer;
    timer.Start();
    for (int i = 0; i < count; ++i) {
        allHash |= uniforms.CalculateHash();
    }
    auto duration = timer.Stop();

    std::cout << std::format("{0} iterations, took {1}, hash = {2} ", count, duration, allHash);



    return 0;
}
