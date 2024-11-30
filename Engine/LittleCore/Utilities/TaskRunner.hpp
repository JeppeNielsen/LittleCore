//
// Created by Jeppe Nielsen on 28/11/2024.
//


#pragma once
#include <iostream>
#include <functional>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace LittleCore {

    class TaskRunner {
    public:
        TaskRunner() ;

        ~TaskRunner();

        // Run an asynchronous task with a callback
        void runAsyncTask(std::function<void()> task, std::function<void()> callback);

        void processTasks();

    private:
        std::queue<std::function<void()>> taskQueue; // Queue to hold callbacks
        std::mutex queueMutex;                      // Mutex for queue access

        void enqueueCallback(const std::function<void()> &callback);
    };

}