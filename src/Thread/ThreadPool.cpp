#include "ThreadPool.h"

ThreadPool::ThreadPool(const size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock lock(queueMutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}


ThreadPool::~ThreadPool() {
    {
        std::unique_lock lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

template<class F>
void ThreadPool::enqueue(F&& task) {
    {
        std::unique_lock lock(queueMutex);
        tasks.emplace(std::forward<F>(task));
    }
    condition.notify_one();
}

