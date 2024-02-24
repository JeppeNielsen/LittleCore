#include <entt/entt.hpp>
#include <iostream>

#include <chrono>
#include <ctime>
#include <cmath>

struct Transform {
    float x;
    float y;
};

struct Mesh {
    int indices;
    int vertices;
};

template<typename T>
struct IResourceLoader {

    virtual ~IResourceLoader() {}

    virtual void Load(T& resource) = 0;
    virtual void Unload(T& unload) = 0;
    virtual bool IsLoaded() = 0;

};

template<typename T>
struct IResourceLoaderFactory {
    virtual ~IResourceLoaderFactory() {}
    virtual IResourceLoader<T>* Create() = 0;
};


template<typename T>
struct ResourceStorage {
    using ResourceLoader = IResourceLoader<T>;

    ResourceLoader* loader;

    T resource;

    ResourceStorage(ResourceLoader* loader) : loader(loader) {
        std::cout << "Resource created \n";
    }

    ~ResourceStorage()  {
        std::cout << "Resource freed \n";
    }

    ResourceStorage(ResourceStorage&&) = delete;
    ResourceStorage(const ResourceStorage&) = delete;
    ResourceStorage& operator=(const ResourceStorage&) = delete;
    ResourceStorage& operator=(ResourceStorage&&) = delete;

    int referenceCount = 0;

    void IncrementReference() {
        ++referenceCount;

        if (referenceCount == 1) {
            //loader->Load(resource);
        }

        std::cout << "IncrementReference, ReferenceCount: " << referenceCount << "\n";
    }

    void DecrementReference() {
        --referenceCount;

        if (referenceCount == 0) {
            //loader->Unload(resource);
        }

        std::cout << "DecrementReference, ReferenceCount: " << referenceCount << "\n";
    }

    T* GetResource() {
        return &resource;
    }

    void Update() {

    }

    bool IsLoaded() {
        return loader->IsLoaded();
    }
};

template<typename T>
struct ResourceHandle {
    ResourceStorage<T>* storage;

    ResourceHandle() : storage(nullptr){
    }

    ResourceHandle(ResourceStorage<T>* storage) {
        this->storage = storage;
        this->storage->IncrementReference();
    }

    ResourceHandle(ResourceHandle&& other) = delete;

    ResourceHandle(const ResourceHandle& other) {
        storage = other.storage;
        if (storage) {
            storage->IncrementReference();
        }
    }

    ResourceHandle& operator= (const ResourceHandle& other) {
        storage = other.storage;
        if (storage != nullptr) {
            storage->IncrementReference();
        }
        return *this;
    }

    ResourceHandle& operator= (ResourceHandle&& other) {
        storage = other.storage;
        if (storage) {
            storage->IncrementReference();
        }
        return *this;
    }

    ~ResourceHandle() {
        if (storage) {
            storage->DecrementReference();
        }
    }

    bool IsLoaded() const {
        return storage->IsLoaded();
    }

    T* operator -> () const {
        return storage ? storage->GetResource() : nullptr;
    }

    operator bool() const {
        return operator->() != nullptr;
    }

};

template<typename T>
struct ResourceLoaderManager {
    std::map<std::string, ResourceStorage<T>> storages;

    ResourceHandle<T> Create(const std::string& id) {
        ResourceStorage<T>* storage;
        auto it = storages.find(id);
        if (it == storages.end()) {
            it = storages.emplace(id, nullptr).first;
        }
        storage = &it->second;
        return ResourceHandle<T>(storage);
    }


};





template <typename T>
struct ResourceComponent {
    ResourceHandle<T> handle;

    T* operator -> () const {
        return handle ? handle.operator->() : (T*)this;
    }
};

struct Texture : ResourceComponent<Texture> {
    std::vector<uint32_t> colors;
};

/*
void TestResource(const TextureComponent& textureComponent) {
    if (textureComponent.texture) {
        std::cout << "Texture is something"<< std::endl;
    } else {
        std::cout << "Texture is null"<< std::endl;
    }
}
 */

int main() {

    ResourceLoaderManager<Texture> textureResources;

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