#include <entt/entt.hpp>
#include <iostream>
#include "ResourceManagement/ResourceComponent.hpp"
#include "ResourceManagement/ResourceLoaderManager.hpp"

using namespace LittleCore;

struct Transform {
    float x;
    float y;
};

struct Mesh {
    int indices;
    int vertices;
};

struct Texture : ResourceComponent<Texture> {
    std::vector<uint32_t> colors;
};

struct TextureLoader : IResourceLoader<Texture> {

    void Load(Texture& resource) override {
        resource.colors.resize(10);
    }

    void Unload(Texture& resource) override {
        resource.colors.clear();
    }

    bool IsLoaded() override {
        return true;
    }

};

struct TextureLoaderFactory : IResourceLoaderFactory<Texture> {

    int numberOfBatches = 0;

    explicit TextureLoaderFactory(int numberOfBatches) :
    numberOfBatches(numberOfBatches) {}

    virtual std::unique_ptr<IResourceLoader<Texture>> Create() override {
        return std::make_unique<TextureLoader>();
    }

};

int main() {

    ResourceLoaderManager<Texture> textureResources(std::make_unique<TextureLoaderFactory>(123));

    entt::registry reg;
    auto ent1 = reg.create();

    reg.emplace<Texture>(ent1);
    reg.get<Texture>(ent1)->colors.push_back(10);
    reg.get<Texture>(ent1)->colors.push_back(20);

    auto size1 = reg.get<Texture>(ent1)->colors.size();

    std::cout << size1 << "\n";

    reg.get<Texture>(ent1).handle = textureResources.Create("Blue.png");

    auto size2 = reg.get<Texture>(ent1)->colors.size();

    std::cout << size2 << "\n";
    /*
    {

        TextureComponent tex1;

        TextureComponent tex4;

        tex4.texture = tex1.texture;

        TestResource(tex1);


        tex1.texture = textureResources.Create("Test");

        TestResource(tex1);



        tex1.texture->colors.push_back(100);

        TextureComponent tex2 = tex1;

        TextureComponent tex3 = tex2;

        {
            TextureComponent newRef;
            newRef.texture = textureResources.Create("Test4");
        }

        std::vector<TextureComponent> components;
        components.resize(10, tex1);

    }
     */

}