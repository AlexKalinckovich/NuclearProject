#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    // Шаблонная функция добавления задачи в очередь
    template<class F>
    void enqueue(F&& task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};

// Определение шаблонной функции должно находиться здесь
template<class F>
void ThreadPool::enqueue(F&& task) {
    {
        std::unique_lock lock(queueMutex);
        tasks.emplace(std::forward<F>(task));
    }
    condition.notify_one();
}

#endif // THREAD_POOL_H
