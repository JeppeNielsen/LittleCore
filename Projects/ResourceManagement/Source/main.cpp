#include <entt/entt.hpp>
#include <iostream>
#include "ResourceManagement/ResourceComponent.hpp"
#include "ResourceManagement/ResourceManager.hpp"

using namespace LittleCore;

struct Transform {
    float x;
    float y;
};

struct Mesh : ResourceComponent<Mesh> {
    int indices;
    int vertices;
};

struct Texture : ResourceComponent<Texture> {
    std::vector<uint32_t> colors;
};

struct MeshLoader : IResourceLoader<Mesh> {

    void Load(Mesh& resource) override {
        resource.vertices = 234;
    }

    void Unload(Mesh& resource) override {

    }

    bool IsLoaded() override {
        return true;
    }

};

struct MeshLoaderFactory : IResourceLoaderFactory<MeshLoader> {
    Loader Create() override {
        return CreateLoader();
    }
};

struct TextureLoader : IResourceLoader<Texture> {

    int numberOfBatches = 0;

    explicit TextureLoader(int numberOfBatches) :
            numberOfBatches(numberOfBatches) {}

    void Load(Texture& resource) override {
        resource.colors.resize(numberOfBatches);
    }

    void Unload(Texture& resource) override {
        resource.colors.clear();
    }

    bool IsLoaded() override {
        return true;
    }

};

struct TextureLoaderFactory : IResourceLoaderFactory<TextureLoader> {

    int numberOfBatches = 0;

    explicit TextureLoaderFactory(int numberOfBatches) :
    numberOfBatches(numberOfBatches) {}

    Loader Create() override {
        return CreateLoader(numberOfBatches);
    }

};

int main() {

    ResourceManager<MeshLoaderFactory, TextureLoaderFactory> resourceManager;
    resourceManager.CreateLoaderFactory<MeshLoaderFactory>();
    resourceManager.CreateLoaderFactory<TextureLoaderFactory>(123);

    entt::registry reg;
    auto ent1 = reg.create();

    reg.emplace<Texture>(ent1);
    reg.get<Texture>(ent1)->colors.push_back(10);
    reg.get<Texture>(ent1)->colors.push_back(20);
    reg.emplace<Mesh>(ent1);

    auto size1 = reg.get<Texture>(ent1)->colors.size();

    std::cout << size1 << "\n";

    reg.get<Texture>(ent1).handle = resourceManager.Create<Texture>("Blue.png");
    reg.get<Mesh>(ent1).handle = resourceManager.Create<Mesh>("Cube.obj");

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