#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <Enemy/Enemy.h>

class Map 
{
    public:
        static Map& getInstance();
        Map();
        void generate();  // Генерация карты
        void draw(sf::RenderWindow& window, const sf::View& view);  // Отрисовка карты
        sf::Vector2f findPlayerStartPosition() const;
        std::vector<std::unique_ptr<Enemy>> spawnEnemies(int count) const; // Спавн врагов
        bool isWallAtPosition(const sf::Vector2f& position) const;
    private:
        int width, height;
        float tileSize;
        std::vector<int> map;  // Одномерный массив для хранения карты
        sf::Texture wallTexture, floorTexture;
        sf::Sprite wallSprite, floorSprite;
        bool isWall(int x, int y) const;  // Проверка, является ли клетка стеной

        void initialMapWithWalls();
        void randomizeMap();
        void smoothMap();
        void smooth(std::vector<int>& newMap) const;
        int countNeighboringWalls(int x,int y) const;
        void addBorders();
};

#endif // MAP_H
