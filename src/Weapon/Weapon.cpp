#include "Weapon.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <string>


constexpr float WEAPON_HEIGHT = 3.0f;
constexpr float PI = 3.14159265f;
constexpr int PI_ANGLE = 180;
constexpr int BULLET_COUNT = 12;
Weapon::Weapon(const std::string& bulletTexturePath,const float bulletSize,BulletOwner owner)
    : weaponDamage(1),
      weaponReloadSpeed(0.25f),
      timeSinceLastShot(0.0f),
      bulletPool(nullptr)
{
    setTexture(bulletTexturePath);
    bulletPool = std::make_unique<BulletPool>(BULLET_COUNT, bulletTexture,bulletSize,owner);  // Инициализация bulletPool после загрузки текстуры
}



void Weapon::setTexture(const std::string& bulletTexturePath)
{
    weaponTextureLeft.loadFromFile(R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Weapons\PopGunLeft.png)");
    weaponTextureRight.loadFromFile(R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Weapons\PopGunRight.png)");
    weaponSprite.setTexture(weaponTextureLeft);

    const sf::FloatRect weaponBounds = weaponSprite.getLocalBounds();
    weaponSprite.setOrigin(weaponBounds.width / 2.0f, weaponBounds.height / 2.0f);

    weaponSprite.setScale(WEAPON_HEIGHT, WEAPON_HEIGHT);

    bulletTexture.loadFromFile(bulletTexturePath);
}

BulletPool *Weapon::getBulletPool() const
{
    return bulletPool.get();
}


void Weapon::update(const sf::Vector2f& playerPosition, const sf::Vector2f& cursorPosition, const float deltaTime, const float scaleValue)
{
    timeSinceLastShot += deltaTime;

    const sf::Vector2f direction = playerPosition - cursorPosition;
    animationAngle = std::atan2(direction.y * scaleValue, direction.x * scaleValue) * PI_ANGLE / PI;
    weaponSprite.setRotation(animationAngle);
    weaponSprite.setPosition(playerPosition);

    bulletPool->update(deltaTime);
}

void Weapon::fire(const sf::Vector2f& playerPosition, const sf::Vector2f& direction,const float scaleValue)
{   
    if (timeSinceLastShot >= weaponReloadSpeed)
    {
        const sf::FloatRect& localBounds = weaponSprite.getLocalBounds();
        
        fireAngle = weaponSprite.getRotation() * PI_ANGLE / PI;
        fireXCoordinate = std::cos(fireAngle) * (localBounds.width) / 2.0f;
        fireYCoordinate = std::sin(fireAngle) * (localBounds.height) / 2.0f;

        if (Bullet* bullet = bulletPool->getBullet())
        {
            const auto pos = sf::Vector2f(fireXCoordinate + playerPosition.x, fireYCoordinate + playerPosition.y);
            bullet->activate(pos, direction, weaponSprite.getRotation(),scaleValue);
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

void Weapon::draw(sf::RenderWindow& window) const
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