#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Graphics.hpp>
#include <memory>
#include "Weapon.h"  // Подключаем оружие

class Enemy {
public:
    Enemy(const sf::Vector2f& position);

    // Основные методы
    void update(const sf::Vector2f& playerPosition, const float deltaTime,const int scaleValue);  // Обновление состояния противника
    void draw(sf::RenderWindow& window);  // Отрисовка противника

    // Проверка, попадает ли игрок в радиус обнаружения
    bool isPlayerInDetectionRange(const sf::Vector2f& playerPosition) const;

    // Генерация случайного направления движения
    void generateRandomMovement(const float deltaTime);

    // Установка радиуса обнаружения
    void setDetectionRadius(const float radius);

    // Проверка на активность противника
    bool isActive() const;

private:
    sf::Sprite sprite;  // Спрайт противника
    sf::Texture enemyTexture;
    sf::Sprite weaponSprite;  // Спрайт оружия
    std::unique_ptr<Weapon> weapon;  // Оружие противника
    sf::Vector2f velocity;  // Скорость и направление движения



    std::vector<sf::IntRect> animationFrames;  // Кадры анимации
    unsigned currentFrame;  // Текущий кадр анимации
    float elapsedTime;  // Время с последнего обновления анимации
    const float frameTime = 0.1f;  // Интервал времени между кадрами

    float detectionRadius;  // Радиус обнаружения игрока
    float moveTimer;  // Таймер для случайного движения
    bool active;  // Флаг активности противника

    float signRotationValue = 1;

    void chasePlayer(const sf::Vector2f& playerPosition, const float deltaTime);  // Движение к игроку
    void randomMovement(const float deltaTime);  // Хаотичное движение
    void loadAnimation();
};

#endif // ENEMY_H