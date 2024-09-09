#include <SFML/Graphics.hpp>
#include "Player/Player.h"
#include "Map/Map.h"

const int MAP_WIDTH = 25;
const int MAP_HEIGHT = 25;
const int TILE_SIZE = 64;

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Player Test");
    sf::View view(sf::FloatRect(0, 0, 800, 600));  // Создаем камеру

    Player& player = Player::getInstance();
    Map map(MAP_WIDTH, MAP_HEIGHT, TILE_SIZE);
    map.generate();
    player.setPosition(map.findPlayerStartPosition());
    std::vector<std::unique_ptr<Enemy>> enemies = map.spawnEnemies(4);
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
        sf::Vector2f cursorPosition = window.mapPixelToCoords(mousePosition);

        // Обрабатываем ввод игрока
        player.handleInput(player,cursorPosition, deltaTime);

        // Обновляем состояние игрока
        player.update(deltaTime, cursorPosition);

        const sf::Vector2f& playerPos = player.getPosition();
        const int playerScaleValue = player.getScaleValue();
        // Обновляем позицию камеры
        view.setCenter(playerPos);
        window.setView(view);

        // Очищаем окно
        window.clear();

        // Отрисовываем карту
        map.draw(window, view);

        // Отрисовываем игрока
        player.draw(window);

        for (auto& enemy : enemies)
        {
            enemy->update(playerPos, deltaTime, playerScaleValue);
            enemy->draw(window);
        }
        // Отображаем содержимое окна
        window.display();
    }

    return 0;
}
