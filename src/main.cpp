#include <condition_variable>
#include <Enemy.h>
#include <mutex>
#include <random>
#include <SFML/Graphics.hpp>
#include "Player/Player.h"
#include "Map/Map.h"
#include "ThreadPool.h"

constexpr int MAP_WIDTH = 25;
constexpr int MAP_HEIGHT = 25;
constexpr int TILE_SIZE = 64;

std::mutex playerMutex;
// Внутри функции отрисовки Health Bar:
void drawHealthBar(sf::RenderWindow &window, const Player &player, const sf::Vector2f &viewPosition) {
    // Получаем размеры View, чтобы отрисовать Health Bar в левом верхнем углу экрана

    // Фон для Health Bar
    sf::RectangleShape hpBarBackground(sf::Vector2f(200, 20));
    hpBarBackground.setFillColor(sf::Color(128, 128, 128));  // Серый цвет
    hpBarBackground.setPosition(viewPosition.x + 20, viewPosition.y + 20);  // Отображаем в левом верхнем углу экрана

    // Полоса здоровья
    float healthPercentage = static_cast<float>(player.getHealth()) / static_cast<float>(player.getMaxHealth());
    sf::RectangleShape hpBar(sf::Vector2f(200 * healthPercentage, 20));
    hpBar.setFillColor(sf::Color::Red);  // Зеленый цвет
    hpBar.setPosition(viewPosition.x + 20, viewPosition.y + 20);

    // Текст здоровья
    sf::Text hpText;
    hpText.setString(std::to_string(player.getHealth()) + "/" + std::to_string(player.getMaxHealth()));
    hpText.setCharacterSize(18);
    hpText.setFillColor(sf::Color::White);
    hpText.setPosition(viewPosition.x + 230, viewPosition.y + 15);

    // Отрисовка
    window.draw(hpBarBackground);
    window.draw(hpBar);
    window.draw(hpText);
}


std::vector<std::unique_ptr<Enemy>> spawnEnemies(const int count,const Map& map)
{
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::random_device rd;
    std::mt19937 rng(rd());
    const float tileSize = map.getTileSize();
    for (int i = 0; i < count; ++i) {
        sf::Vector2f spawnPosition;
        do {
            const unsigned x = rng() % map.getWidth();
            const unsigned y = rng() % map.getHeight();
            spawnPosition = sf::Vector2f(x * tileSize, y * tileSize);
        } while (map.isWall(spawnPosition.x / tileSize, spawnPosition.y / tileSize));

        enemies.emplace_back(std::make_unique<Enemy>(spawnPosition));
    }

    return enemies;
}


// Функция обновления игрока и врагов
void updateGameEntities(Player &player, std::vector<std::unique_ptr<Enemy>> &enemies,
                        const float deltaTime, ThreadPool &pool) {
    // Обновление врагов через пул потоков
    for (auto& enemy : enemies) {
        pool.enqueue([&enemy, deltaTime, &player] {
            if(enemy->isActive())
            {
                enemy->update(player, deltaTime);
            }
        });
    }
}
void updatePlayer(Player& player, const sf::Vector2f& cursorPosition, float deltaTime, ThreadPool& pool)
{
    pool.enqueue([&player, cursorPosition, deltaTime]
    {
        player.handleInput(player, cursorPosition, deltaTime);
        player.update(deltaTime, cursorPosition);
    });
}

void handlePlayerBulletCollision(const Player& player,  std::vector<std::unique_ptr<Enemy>>& enemies)
{
    const BulletPool* playerBullets = player.getWeapon()->getBulletPool();
    // Обработка пуль игрока, попадающих во врагов
    for (const auto& bullet : playerBullets->getBullets()) {
        if (!bullet->isActive()) continue;

        // Проверка на столкновение с каждым врагом
        for (const auto& enemy : enemies) {
            if (enemy->isActive() && bullet->checkCollision(enemy->getSprite().getGlobalBounds())) {
                enemy->takeDamage(bullet.get());
                break;  // Прекращаем проверку, т.к. пуля уже попала в цель
            }
        }
    }
}

void handleEnemyBulletCollision(Player& player, const std::vector<std::unique_ptr<Enemy>>& enemies)
{
    for(const auto& enemy : enemies)
    {
        const BulletPool* enemyBullets = enemy->getWeapon()->getBulletPool();
        // Обработка пуль врагов, попадающих в игрока
        for (auto& bullet : enemyBullets->getBullets()) {

            if(!bullet->isActive()) continue;

            if (enemy->isActive())
            {
                if(bullet -> getOwner() == PLAYER_BULLET)
                {
                    if(bullet->checkCollision(enemy->getSprite().getGlobalBounds()))
                    {
                        enemy->takeDamage(bullet.get());
                    }
                }
                else if(bullet->checkCollision(player.getSprite().getGlobalBounds()))
                {
                    player.takeDamage(bullet.get());
                }
            }
        }
    }
}

void handleBulletCollisions(Player& player,  std::vector<std::unique_ptr<Enemy>>& enemies,ThreadPool& pool)
{
    pool.enqueue([&player, &enemies]
    {
        handlePlayerBulletCollision(player, enemies);
    });
    pool.enqueue([&player, &enemies]
    {
        handleEnemyBulletCollision(player, enemies);
    });
}


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Player Test");
    sf::View view(sf::FloatRect(0, 0, 800, 600));  // Камера

    Player& player = Player::getInstance();
    Map* map = &Map::getInstance();
    map->generate();
    std::vector<std::unique_ptr<Enemy>> enemies = spawnEnemies(10,*map);

    sf::Clock clock;
    player.setPosition(map->getPlayerStartPosition());
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
        updateGameEntities(player, enemies, deltaTime, pool);
        updatePlayer(player, cursorPosition, deltaTime, pool);
        handleBulletCollisions(player,enemies,pool);
        // Обновляем позицию камеры
        view.setCenter(player.getPosition());
        window.setView(view);

        // Очищаем окно
        window.clear();

        // Отрисовываем карту, игрока и врагов
        map->draw(window, view);
        const sf::Vector2f viewPosition = view.getCenter() - view.getSize() / 2.f;
        const sf::FloatRect viewBounds(viewPosition, view.getSize());
        for (const auto& enemy : enemies)
        {
            if(viewBounds.intersects(enemy->getSprite().getGlobalBounds()))
            {
                enemy->draw(window);
            }
        }

        player.draw(window);
        drawHealthBar(window,player,viewPosition);
        // Отображаем содержимое окна
        window.display();
    }

    return 0;
}
