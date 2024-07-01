//
// Created by Jeppe Nielsen on 24/05/2024.
//


#pragma once
#include <experimental/coroutine>
#include <vector>

namespace LittleCore {

    class Fiber {
    public:
        bool Step();
        bool IsTerminated();

    private:
        Fiber();

        struct Promise;
        using CoroutineHandle = std::experimental::coroutine_handle<Promise>;

        struct Promise {

            Promise();

            ~Promise();

            Fiber get_return_object();

            void unhandled_exception() noexcept;

            void return_void() noexcept;

            std::experimental::suspend_always yield_value(int val);

            std::experimental::suspend_always yield_value(Fiber fiber);

            std::experimental::suspend_always initial_suspend() noexcept;

            std::experimental::suspend_always final_suspend() noexcept;

            bool Step();

            bool IsTerminated();

            Promise *FindRoot(Promise *current);

            CoroutineHandle coroutine;
            std::vector<Promise *> stack;
            Promise *parent;
            bool didCreate;
        };

        Promise *promise = nullptr;
    public:
        using promise_type = Promise;
    };
}