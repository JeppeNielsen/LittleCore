//
// Created by Jeppe Nielsen on 25/02/2024.
//

#pragma once
#include "ResourceStorage.hpp"

namespace LittleCore {

    template<typename T>
    struct ResourceHandle {
        ResourceStorage<T>* storage;

        ResourceHandle() : storage(nullptr){
        }

        explicit ResourceHandle(ResourceStorage<T>* storage) {
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
            return storage ? storage->IsLoaded() : false;
        }

        T* operator -> () const {
            return storage && storage->IsLoaded() ? storage->GetResource() : nullptr;
        }

        operator bool() const {
            return operator->() != nullptr;
        }

    };

}