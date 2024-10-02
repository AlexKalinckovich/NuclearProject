#ifndef MAP_H
#define MAP_H

#include <SFML/Graphics.hpp>
#include <vector>

class Enemy;

class Map {
public:
    // Возвращаем ссылку на единственный экземпляр класса
    static Map& getInstance()
    {
        static Map instance;  // Гарантированная статическая инициализация при первом вызове
        return instance;
    }

    int getWidth() const;
    int getHeight() const;
    float getTileSize() const;

    void generate();  // Генерация карты


    void draw(sf::RenderWindow& window, const sf::View& view);  // Отрисовка карты

    bool isWallAtPosition(const sf::Vector2f& position) const;
    const std::vector<int>& getMap() const;  // Getter для карты
    sf::Vector2f getPlayerStartPosition() const;

    bool isWall(int x, int y) const;  // Проверка, является ли клетка стеной

private:
    int width, height;
    float tileSize;
    std::vector<int> map;  // Одномерный массив для хранения карты
    sf::Texture wallTexture, floorTexture;
    sf::Sprite wallSprite, floorSprite;
    sf::Vector2f playerStartPosition;
    // Приватный конструктор
    Map();
    sf::Vector2f findPlayerStartPosition() const;

    void initialMapWithWalls();
    void randomizeMap();
    void smoothMap();
    void smooth(std::vector<int>& newMap) const;
    int countNeighboringWalls(int x, int y) const;
    void addBorders();
    void generateLabyrinth(int x, int y);

    std::vector<std::vector<bool>> findAccessibleAreas(const sf::Vector2f &start) const;

    // Запрещаем копирование и присваивание
    Map(const Map&) = delete;
    Map& operator=(const Map&) = delete;
};

#endif // MAP_H
