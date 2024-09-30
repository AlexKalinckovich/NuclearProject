#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <functional>
#include <array>
#include <Map.h>

#include "Weapon/Weapon.h"
#include "Ability/Ability.h"
#include <memory>

class Ability;
// Перечисление состояний игрока
enum State { IDLE = 0, WALKING = 1, HURT = 2, ROLLING = 3, SHIELD = 4 };

struct Animation
{
    sf::Texture texture;
    std::vector<sf::IntRect> frames;
    int frameCount;
};

class Player 
{
public:
    static Player& getInstance();

    // Основные методы
    void update(float deltaTime, const sf::Vector2f& cursorPosition);
    void draw(sf::RenderWindow& window) const;
    void handleInput(Player& player,const sf::Vector2f& cursorPosition, float deltaTime);
    // Установка/получение здоровья и скорости
    void setHealth(int health);

    unsigned getHealth() const;

    void setSpeed(float speed);
    float getSpeed() const;

    void setState(State state);
    State getState() const;

    void setSpecialActive(bool isActive);
    bool getSpecialActive() const;

    void setNormalBounds();

    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& position);

    float getScaleValue() const;
    void setMap(const Map& map);
private:
    Player();
    ~Player();


    bool isSpecialActive = false;
    Map map;
    Player(const Player&);
    Player& operator=(const Player&) const;

    // Поля для анимации
    sf::Sprite sprite;
    sf::FloatRect playerBounds;

    unsigned currentFrame = 0;
    float elapsedTime = 0.0f;

    float signRotationValue = 1.0f;

    State state = IDLE;
    State previousState = IDLE;

    std::function<void()> stateChanger[5];

    unsigned health;
    float speed;

    std::vector<std::unique_ptr<Weapon>> weapons;
    Weapon* currentWeapon;

    std::vector<std::unique_ptr<Ability>> abilities;
    Ability* currentAbility = nullptr;
    unsigned currentAbilityIndex = 0;

    // Массив анимаций вместо map
    std::array<Animation, 5> animations;

    void changeAbility();
    void loadAnimation(State state, const std::string& texturePath, int frameCount, int frameWidth, int frameHeight, int gap);
    void initAnimations();
    void switchAbility();
    void smoothMove(const sf::Vector2f& targetPosition, float deltaTime);
};

#endif // PLAYER_H
