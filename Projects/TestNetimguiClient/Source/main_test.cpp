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

using namespace LittleCore;



struct RenderableObject {
    ResourceHandle<FontResource> font;
    ResourceHandle<FontResource> largeFont;
};

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




int main() {

    ResourcePathMapper resourcePathMapper;

    std::string path = "/Users/jeppe/Jeppes/LittleCore/Projects/TestNetimguiClient/Source/Assets/";
    resourcePathMapper.RefreshFromRootPath(path);

    DefaultResourceManager resourceManager(resourcePathMapper);

    glz::meta<ResourceHandle<FontResource>>::resourceManager = &resourceManager;

    resourceManager.CreateLoaderFactory<FontResourceLoaderFactory>();

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
