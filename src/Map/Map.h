#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <Enemy/Enemy.h>

class Map 
{
    public:
        Map(int width, int height, int tileSize);
        void generate();  // Генерация карты
        void draw(sf::RenderWindow& window, const sf::View& view);  // Отрисовка карты
        bool isWall(int x, int y) const;  // Проверка, является ли клетка стеной
        sf::Vector2f findPlayerStartPosition();  // Поиск начальной позиции игрока
        std::vector<std::unique_ptr<Enemy>> spawnEnemies(int count); // Спавн врагов
    private:
        int width, height, tileSize;
        std::vector<int> map;  // Одномерный массив для хранения карты
        sf::Texture wallTexture, floorTexture;
        sf::Sprite wallSprite, floorSprite;
};

#endif // MAP_H
