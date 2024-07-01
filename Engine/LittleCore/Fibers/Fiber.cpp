//
// Created by Jeppe Nielsen on 24/05/2024.
//

#include "Fiber.hpp"
#include <iostream>

bool Fiber::Step() {
    return promise->Step();
}

bool Fiber::IsTerminated() {
    return promise->IsTerminated();
}

Fiber::Fiber() {
    std::cout << "Fiber ctor \n";
}

Fiber::Promise::Promise() : parent(nullptr), didCreate(false) {
    coroutine = CoroutineHandle::from_promise(*this);
    std::cout << "Promise ctor, promise:"<<this<<"\n";
}


Fiber::Promise::~Promise() {
    std::cout << "Promise dtor, promise:"<<this<<"\n";
}

Fiber Fiber::Promise::get_return_object() {

    Fiber fiber;
    std::cout << "get_return_object:"<<this<<"\n";
    fiber.promise = this;
    stack.push_back(this);

    didCreate = true;
    return fiber;
}

void Fiber::Promise::unhandled_exception() noexcept {

}

void Fiber::Promise::return_void() noexcept {

    auto root = FindRoot(this);
    root->stack.pop_back();
    if (!root->stack.empty()) {
        root->stack.back()->coroutine.resume();
    }
    /*if (parent != nullptr) {
        auto root = FindRoot(this);
        root->stack.pop_back();
    }*/
    //stepOneExtra = true;
}

std::experimental::suspend_always Fiber::Promise::yield_value(int val) {
    return {};
}

std::experimental::suspend_always Fiber::Promise::yield_value(Fiber fiber) {
/*
    auto fromRoot = FindRoot(this);

    if (!fromRoot->didCreate) {
        return {};
    }
*/
    std::cout << "yield_value, promise:"<<this<<"\n";
    fiber.promise->parent = this;

    auto root = FindRoot(fiber.promise);
    root->stack.push_back(fiber.promise);
    root->stack.back()->coroutine.resume();

    didCreate = false;

    return {};
}

std::experimental::suspend_always Fiber::Promise::initial_suspend() noexcept {
    return {};
}

std::experimental::suspend_always Fiber::Promise::final_suspend() noexcept {
    return {};
}

bool Fiber::Promise::Step() {
    if (stack.empty()) {
        return false;
    }
    Promise* p = stack.back();
    didCreate = false;
    p->coroutine.resume();
    return !IsTerminated();
}

bool Fiber::Promise::IsTerminated() {
    if (stack.empty()) {
        return true;
    }
    return stack.back()->coroutine.done();
}

Fiber::Promise *Fiber::Promise::FindRoot(Promise* current) {
    while (current->parent != nullptr) {
        current = current->parent;
    }
    return current;
}
