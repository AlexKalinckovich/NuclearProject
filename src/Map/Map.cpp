#include "Map.h"
#include <random>
#include <algorithm>
#include <queue>

constexpr int MAX_NEIGHBOR_WALLS = 5;
constexpr int SMOOTH_ITERATIONS = 10;
constexpr int MAP_WIDTH = 25;
constexpr int MAP_HEIGHT = 25;
constexpr int TILE_SIZE = 64;

Map::Map()
    : width(MAP_WIDTH), height(MAP_HEIGHT), tileSize(TILE_SIZE), map(width * height, 1)
{
    wallTexture.loadFromFile(R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Map\wall.png)");
    floorTexture.loadFromFile(R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Map\floor.png)");

    wallSprite.setTexture(wallTexture);
    wallSprite.setScale(4.0f, 4.0f);
    floorSprite.setTexture(floorTexture);
    floorSprite.setScale(4.0f, 4.0f);
}

void Map::generate() {
    initialMapWithWalls(); // Заполняем карту стенами
    randomizeMap();            // Первичное случайное заполнение
    smoothMap();              // Несколько итераций сглаживания
    addBorders();
}

void Map::initialMapWithWalls()
{
    std::fill(map.begin(), map.end(), 1); // 1 - Стены
}

void Map::randomizeMap()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            map[y * width + x] = rng() % 100 < 45 ? 1 : 0;
        }
    }
}

int Map::countNeighboringWalls(const int x, const int y) const
{
    int count = 0;
    for(int ny = -1; ny <= 1; ++ny)
    {
        for(int nx = -1; nx <= 1; ++nx)
        {
            if (map[(y + ny) * width + (x + nx)] == 1)
            {
                count++;
            }
        }
    }
    return count;
}


void Map::smoothMap()
{

    for(int i = 0;i < SMOOTH_ITERATIONS;++i)
    {
        std::vector<int> newMap = map;
        smooth(newMap);
        map = newMap;
    }
}

void Map::smooth(std::vector<int>& newMap) const
{
    for (int y = 1; y < height - 1; ++y)
    {
        for (int x = 1; x < width - 1; ++x)
        {
            if (const int neighborWalls = countNeighboringWalls(x, y); neighborWalls >= MAX_NEIGHBOR_WALLS)
            {
                newMap[y * width + x] = 1;  // Превращаем клетку в стену
            }
            else
            {
                newMap[y * width + x] = 0;  // Оставляем пол
            }
        }
    }
}

void Map::addBorders()
{
    for(int x = 0; x < width; ++x)
    {
        map[x] = map[(height - 1) * width + x] = 1;
    }
    for(int y = 0; y < height; ++y)
    {
        map[y * width] = map[y * width + (width - 1)] = 1;
    }
}


void Map::draw(sf::RenderWindow& window, const sf::View& view)
{
    const sf::Vector2f& viewCenter = view.getCenter();
    const sf::Vector2f& viewSize = view.getSize();
    
    const int startX = std::max(0, static_cast<int>((viewCenter.x - viewSize.x / 2) / tileSize));
    const int startY = std::max(0, static_cast<int>((viewCenter.y - viewSize.y / 2) / tileSize));
    const int endX = std::min(width, static_cast<int>((viewCenter.x + viewSize.x / 2) / tileSize) + 1);
    const int endY = std::min(height, static_cast<int>((viewCenter.y + viewSize.y / 2) / tileSize) + 1);

    for (int i = startX; i < endX; i++)
    {
        for (int j = startY; j < endY; j++)
        {
            sf::Sprite& sprite = map[j * width + i] == 1 ? wallSprite : floorSprite;
            sprite.setPosition(static_cast<float>(i) * tileSize, static_cast<float>(j) * tileSize);
            window.draw(sprite);
        }
    }
}

sf::Vector2f Map::findPlayerStartPosition() const {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution distX(0, width - 1);
    std::uniform_int_distribution distY(0, height - 1);

    int x, y;
    do 
    {
        x = distX(rng);
        y = distY(rng);
    } while (isWall(x, y));  // Проверяем, что игрок не окажется в стене

    return {(static_cast<float>(x) * tileSize), (static_cast<float>(y) * tileSize)};
}

Map& Map::getInstance()
{
    static Map map;
    return map;
}


bool Map::isWallAtPosition(const sf::Vector2f& position) const {
    const int tileX = static_cast<int>(position.x / tileSize);
    const int tileY = static_cast<int>(position.y / tileSize);

    if (tileX < 0 || tileY < 0 || tileX >= width || tileY >= height) {
        return true;  // Стена на границе карты
    }

    return map[tileY * width + tileX] == 1;  // 1 = стена, 0 = пол
}


bool Map::isWall(const int x, const int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return true; // Считаем, что за пределами карты находится стена
    }
    return map[y * width + x] == 1;
}


std::vector<std::unique_ptr<Enemy>> Map::spawnEnemies(const int count) const
{
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int i = 0; i < count; ++i) {
        sf::Vector2f spawnPosition;
        do {
            const unsigned x = rng() % width;
            const unsigned y = rng() % height;
            spawnPosition = sf::Vector2f(x * tileSize, y * tileSize);
        } while (isWall(spawnPosition.x / tileSize, spawnPosition.y / tileSize));

        enemies.emplace_back(std::make_unique<Enemy>(spawnPosition));
    }

    return enemies;
}

