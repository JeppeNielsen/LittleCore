//
// Created by Jeppe Nielsen on 25/10/2025.
//

#include "PrefabResourceLoader.hpp"
#include "RegistryHelper.hpp"

using namespace LittleCore;


void PrefabResourceLoader::Load(PrefabResource& resource) {

}

void PrefabResourceLoader::Unload(PrefabResource& resource) {
    registry.clear();
}

bool PrefabResourceLoader::IsLoaded() {
    return false;
}

void PrefabResourceLoader::Reload(PrefabResource& resource) {

}

PrefabResourceLoader::PrefabResourceLoader(RegistrySerializerBase& registrySerializer) : registrySerializer(registrySerializer) {

}
