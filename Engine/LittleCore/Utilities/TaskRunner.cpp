//
// Created by Jeppe Nielsen on 28/11/2024.
//

#include "TaskRunner.hpp"

using namespace LittleCore;

TaskRunner::TaskRunner() {

}

TaskRunner::~TaskRunner() {
}

void TaskRunner::runAsyncTask(std::function<void()> task, std::function<void()> callback) {
    // Launch the task asynchronously

    std::thread{
            [task, callback, this]() {
                task();
                // Enqueue the callback to run in the main thread

                enqueueCallback(callback);
            }
    }.detach();

    /*auto handle = std::async(std::launch::async, [this, task, callback]() {
        // Perform the task
        task();
        // Enqueue the callback to run in the main thread
        enqueueCallback(callback);
    });
     */
}

void TaskRunner::enqueueCallback(const std::function<void()> &callback) {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(callback);
}

void TaskRunner::processTasks()  {
    while (!taskQueue.empty()) {
        std::unique_lock<std::mutex> lock(queueMutex);

        while (!taskQueue.empty()) {
            auto task = taskQueue.front();
            taskQueue.pop();
            task();
        }
    }
}
