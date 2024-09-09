#include "Map.h"
#include <random>
#include <algorithm>
#include <queue>


Map::Map(int width, int height, int tileSize)
    : width(width), height(height), tileSize(tileSize), map(width * height, 1) 
{
    wallTexture.loadFromFile("E:\\NuclearProject\\icons\\Map\\wall.png");
    floorTexture.loadFromFile("E:\\NuclearProject\\icons\\Map\\floor.png");

    wallSprite.setTexture(wallTexture);
    wallSprite.setScale(4.0f, 4.0f);
    floorSprite.setTexture(floorTexture);
    floorSprite.setScale(4.0f, 4.0f);
}

void Map::generate() {
    // Генерация с использованием случайных блужданий или алгоритма cellular automata
    std::fill(map.begin(), map.end(), 1);  // Заполняем карту стенами
    std::random_device rd;
    std::mt19937 rng(rd());

    // Первый этап - случайное заполнение карты
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            map[y * width + x] = (rng() % 100 < 45) ? 1 : 0;  // 45% вероятность стены
        }
    }

    // Второй этап - применение правил клеточного автомата
    for (int i = 0; i < 4; ++i)
    {  // Несколько итераций для сглаживания карты
        std::vector<int> newMap = map;

        for (int y = 1; y < height - 1; ++y)
        {
            for (int x = 1; x < width - 1; ++x)
            {
                int neighborWalls = 0;
                for (int ny = -1; ny <= 1; ++ny)
                {
                    for (int nx = -1; nx <= 1; ++nx)
                    {
                        if (map[(y + ny) * width + (x + nx)] == 1)
                        {
                            neighborWalls++;
                        }
                    }
                }
                
                if (neighborWalls >= 5)
                {
                    newMap[y * width + x] = 1;  // Превращаем клетку в стену
                } 
                else 
                {
                    newMap[y * width + x] = 0;  // Оставляем пол
                }
            }
        }
        map = newMap;  // Обновляем карту
    }

    // Добавляем границы
    for (int x = 0; x < width; ++x)
    {
        map[x] = map[(height - 1) * width + x] = 1;
    }
    for (int y = 0; y < height; ++y)
    {
        map[y * width] = map[y * width + (width - 1)] = 1;
    }
}

void Map::draw(sf::RenderWindow& window, const sf::View& view)
{
    const sf::Vector2f& viewCenter = view.getCenter();
    const sf::Vector2f& viewSize = view.getSize();
    
    const int startX = std::max(0, int((viewCenter.x - viewSize.x / 2) / tileSize));
    const int startY = std::max(0, int((viewCenter.y - viewSize.y / 2) / tileSize));
    const int endX = std::min(width, int((viewCenter.x + viewSize.x / 2) / tileSize) + 1);
    const int endY = std::min(height, int((viewCenter.y + viewSize.y / 2) / tileSize) + 1);

    for (int i = startX; i < endX; i++)
    {
        for (int j = startY; j < endY; j++)
        {
            sf::Sprite& sprite = (map[j * width + i] == 1) ? wallSprite : floorSprite;
            sprite.setPosition(i * tileSize, j * tileSize);
            window.draw(sprite);
        }
    }
}

sf::Vector2f Map::findPlayerStartPosition()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);

    int x, y;
    do 
    {
        x = distX(rng);
        y = distY(rng);
    } while (isWall(x, y));  // Проверяем, что игрок не окажется в стене

    return sf::Vector2f(x * tileSize, y * tileSize);
}

bool Map::isWall(int x, int y) const
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return true; // Считаем, что за пределами карты находится стена
    }
    return map[y * width + x] == 1;
}


std::vector<std::unique_ptr<Enemy>> Map::spawnEnemies(int count)
{
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::random_device rd;
    std::mt19937 rng(rd());

    for (int i = 0; i < count; ++i) {
        sf::Vector2f spawnPosition;
        do {
            int x = rng() % width;
            int y = rng() % height;
            spawnPosition = sf::Vector2f(x * tileSize, y * tileSize);
        } while (isWall(spawnPosition.x / tileSize, spawnPosition.y / tileSize));

        enemies.emplace_back(std::make_unique<Enemy>(spawnPosition));
    }

    return enemies;
}