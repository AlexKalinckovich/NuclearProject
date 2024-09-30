#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <SFML/Graphics.hpp>
#include "Player/Player.h"
#include "Map/Map.h"

constexpr int MAP_WIDTH = 25;
constexpr int MAP_HEIGHT = 25;
constexpr int TILE_SIZE = 64;

class ThreadPool {
public:
    explicit ThreadPool(const size_t threads) {
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

    template<class F>
    void enqueue(F&& task) {
        {
            std::unique_lock lock(queueMutex);
            tasks.emplace(std::forward<F>(task));
        }
        condition.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers) {
            worker.join();
        }
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop = false;
};

std::mutex playerMutex;

// Функция обновления игрока и врагов
void updateGameEntities(Player& player, std::vector<std::unique_ptr<Enemy>>& enemies,
                        const sf::Vector2f& cursorPosition,const float deltaTime, ThreadPool& pool) {
    // Обновление физики игрока через пул потоков
    pool.enqueue([&player, cursorPosition, deltaTime] {
        std::lock_guard lock(playerMutex);
        player.handleInput(player, cursorPosition, deltaTime);
        player.update(deltaTime, cursorPosition);
    });

    // Обновление врагов через пул потоков
    for (auto& enemy : enemies) {
        pool.enqueue([&enemy, deltaTime, &player] {
            enemy->update(player.getPosition(), deltaTime);
        });
    }
}



int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Player Test");
    sf::View view(sf::FloatRect(0, 0, 800, 600));  // Камера

    Player& player = Player::getInstance();
    Map map = Map::getInstance();
    map.generate();
    std::vector<std::unique_ptr<Enemy>> enemies = map.spawnEnemies(4);

    sf::Clock clock;
    player.setPosition(map.findPlayerStartPosition());
    player.setMap(map);
    ThreadPool pool(4);  // Пул потоков с 4 потоками
    window.setVerticalSyncEnabled(true);
    // Основной игровой цикл
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        const float deltaTime = clock.restart().asSeconds();
        const sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        const sf::Vector2f cursorPosition = window.mapPixelToCoords(mousePosition);

        // Обновляем физику игрока и врагов в пуле потоков
        updateGameEntities(player, enemies, cursorPosition, deltaTime, pool);

        // Обновляем позицию камеры
        view.setCenter(player.getPosition());
        window.setView(view);

        // Очищаем окно
        window.clear();

        // Отрисовываем карту, игрока и врагов
        map.draw(window, view);
        player.draw(window);
        for (const auto& enemy : enemies) {
            enemy->draw(window);
        }

        // Отображаем содержимое окна
        window.display();
    }

    return 0;
}
