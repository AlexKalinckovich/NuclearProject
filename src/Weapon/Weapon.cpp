#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>

const float WEAPON_HEIGHT = 3.0f;
const float PI = 3.14159265f;
const int PI_ANGLE = 180;

Weapon::Weapon(const std::string& bulletTexturePath,const float bulletSize)
    : weaponReloadSpeed(0.25f),
      weaponDamage(1),
      timeSinceLastShot(0.0f),
      bulletPool(nullptr)
{
    setTexture(bulletTexturePath);
    bulletPool = std::make_unique<BulletPool>(10, bulletTexture,bulletSize);  // Инициализация bulletPool после загрузки текстуры
}



void Weapon::setTexture(const std::string& bulletTexturePath)
{
    weaponTextureLeft.loadFromFile("E:\\NuclearProject\\icons\\Weapons\\PopGunLeft.png");
    weaponTextureRight.loadFromFile("E:\\NuclearProject\\icons\\Weapons\\PopGunRight.png");
    weaponSprite.setTexture(weaponTextureLeft);

    sf::FloatRect weaponBounds = weaponSprite.getLocalBounds();
    weaponSprite.setOrigin(weaponBounds.width / 2.0f, weaponBounds.height / 2.0f);

    weaponSprite.setScale(WEAPON_HEIGHT, WEAPON_HEIGHT);

    bulletTexture.loadFromFile(bulletTexturePath);
}

void Weapon::update(const sf::Vector2f& playerPosition, const sf::Vector2f& cursorPosition, const float deltaTime, const int scaleValue)
{
    timeSinceLastShot += deltaTime;

    sf::Vector2f direction = playerPosition - cursorPosition;
    animationAngle = atan2(direction.y * scaleValue, direction.x * scaleValue) * PI_ANGLE / PI;
    weaponSprite.setRotation(animationAngle);
    weaponSprite.setPosition(playerPosition);

    bulletPool->update(deltaTime);
}

void Weapon::fire(const sf::Vector2f& playerPosition, const sf::Vector2f& direction,const int scaleValue)
{   
    if (timeSinceLastShot >= weaponReloadSpeed)
    {
        const sf::FloatRect& localBounds = weaponSprite.getLocalBounds();
        
        fireAngle = weaponSprite.getRotation() * PI_ANGLE / PI;
        fireXCoordinate = std::cos(fireAngle) * (localBounds.width) / 2.0f;
        fireYCoordinate = std::sin(fireAngle) * (localBounds.height) / 2.0f; 

        Bullet* bullet = bulletPool->getBullet();
        if (bullet)
        {
            bullet->activate(sf::Vector2f(fireXCoordinate + playerPosition.x, fireYCoordinate + playerPosition.y), direction, weaponSprite.getRotation(),scaleValue);
            timeSinceLastShot = 0.0f;
        }
    }
}

void Weapon::mirroredLeft()
{
    weaponSprite.setTexture(weaponTextureLeft);
}

void Weapon::mirroredRight()
{
    weaponSprite.setTexture(weaponTextureRight);
}

void Weapon::draw(sf::RenderWindow& window) 
{
    window.draw(weaponSprite);
    bulletPool->draw(window);  // Отрисовка всех активных пуль из пулла
}

sf::Sprite Weapon::getWeaponSprite() const
{
    return weaponSprite;
}

void Weapon::setPosition(const sf::Vector2f& position)
{
    weaponSprite.setPosition(position);
}