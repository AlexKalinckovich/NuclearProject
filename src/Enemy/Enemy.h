#ifndef ENEMY_H
#define ENEMY_H

#include <array>
#include <SFML/Graphics.hpp>
#include <memory>
#include <Player.h>

#include "Weapon.h"  // Подключаем оружие
#include "Map.h"
class Map;
class Weapon;

enum enemyState { ENEMY_WALKING = 0, ENEMY_HURT = 1, ENEMY_DEAD = 2};

struct enemyAnimation
{
    sf::Texture texture;
    std::vector<sf::IntRect> frames;
    int frameCount;
};

class Enemy {
public:
    explicit Enemy(const sf::Vector2f& position);

    void update(const Player &player, float deltaTime);  // Обновление состояния противника
    void activeUpdate(const sf::Vector2f &playerPosition, float deltaTime, bool isPlayerAlive);

    void draw(sf::RenderWindow& window) const;  // Отрисовка противника

    void takeDamage(Bullet* bullet);
    bool isActive() const;
    void setDetectionRadius(float radius);

    Weapon* getWeapon() const;
    sf::Sprite getSprite() const;

    static int getScore() { return score; }  // Статический метод для получения очков
    static sf::Vector2f getLastEnemyPosition() {return lastEnemyPosition;};  // Получить позицию последнего убитого врага
    static void resetScore(){score = 0;}

private:
    sf::Sprite sprite;
    std::unique_ptr<Weapon> weapon;
    std::array<enemyAnimation, 3> animations;
    sf::Vector2f velocity;

    static int score;  // Статическая переменная для очков
    static sf::Vector2f lastEnemyPosition;  // Статическая переменная для позиции последнего убитого врага

    unsigned currentFrame = 0;
    float elapsedTime = 0.0f;
    float damageAnimationTime = 0.0f;
    const float frameTime = 0.1f;
    float detectionRadius = 300.0f;
    float moveTimer = 0.0f;
    float signRotationValue = 1;
    bool active = true;

    int health = 10;
    enemyState state = ENEMY_WALKING;
    enemyState previousState = ENEMY_WALKING;

    Map* map = nullptr;

    // Логические блоки
    void handleRotation(const sf::Vector2f &playerPosition,const sf::Vector2f& position);
    void handleState();
    void handleEnemyDamage(float deltaTime);
    void handlePlayerMovement(const sf::Vector2f &playerPosition, const sf::Vector2f &position, float deltaTime, bool isPlayerAlive);
    // Вспомогательные функции
    void spriteInit(const sf::Vector2f& position);
    void loadAnimation(enemyState state, const std::string& texturePath, int frameCount, int frameWidth, int frameHeight, int gap);
    void chasePlayer(const sf::Vector2f& playerPosition);
    void randomMovement(float deltaTime);
    bool isPlayerInDetectionRange(const sf::Vector2f& playerPosition) const;
    void deactivateBullets() const;
};

#endif // ENEMY_H