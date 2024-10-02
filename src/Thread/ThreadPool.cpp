#include "ThreadPool.h"

ThreadPool::ThreadPool(const size_t threads) {
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock lock(queueMutex); // Блокируем очередь задач
                    condition.wait(lock, [this] { return stop || !tasks.empty(); }); // Ждем задачи или завершение работы
                    if (stop && tasks.empty()) return; // Если поток завершен и задач больше нет
                    task = std::move(tasks.front());  // Извлекаем задачу
                    tasks.pop();
                }
                task(); // Выполняем задачу
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock lock(queueMutex);
        stop = true;
    }
    condition.notify_all(); // Уведомляем все потоки о завершении
    for (std::thread &worker : workers) {
        worker.join(); // Ожидаем завершение всех потоков
    }
}

