//
// Created by Jeppe Nielsen on 24/05/2024.
//

#include "Fiber.hpp"

using namespace LittleCore;

bool Fiber::Step() {
    return promise->Step();
}

bool Fiber::IsTerminated() {
    return promise->IsTerminated();
}

Fiber::Fiber() {}

Fiber::Promise::Promise() : parent(nullptr) {
    coroutine = CoroutineHandle::from_promise(*this);
}

Fiber::Promise::~Promise() {}

Fiber Fiber::Promise::get_return_object() {

    Fiber fiber;
    fiber.promise = this;
    stack.push_back(this);

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
}

std::suspend_always Fiber::Promise::yield_value(int val) {
    return {};
}

std::suspend_always Fiber::Promise::yield_value(Fiber fiber) {
    fiber.promise->parent = this;

    auto root = FindRoot(fiber.promise);
    root->stack.push_back(fiber.promise);
    root->stack.back()->coroutine.resume();
    return {};
}

std::suspend_always Fiber::Promise::initial_suspend() noexcept {
    return {};
}

std::suspend_always Fiber::Promise::final_suspend() noexcept {
    return {};
}

bool Fiber::Promise::Step() {
    if (stack.empty()) {
        return false;
    }
    Promise* p = stack.back();
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