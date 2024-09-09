#include "Bullet.h"
#include <SFML/Graphics.hpp>
#include <cmath>

const float BULLET_SPEED = 500.0f;
const float BULLET_LIFETIME = 2.0f;

Bullet::Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, const sf::Texture& texture, const float angle,const float bulletSize)
    : isActive(false), elapsedTime(0.0f), lifeTime(BULLET_LIFETIME)
{

    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setScale(bulletSize, bulletSize);
    this->bulletSize = bulletSize;

    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = BULLET_SPEED * direction / length;

    sprite.setRotation(angle);
}

void Bullet::activate(const sf::Vector2f& position, const sf::Vector2f& direction, const float angle,const int scaleValue)
{
    sprite.setPosition(position);
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = BULLET_SPEED * direction / length;
    sprite.setRotation(angle);
    if(scaleValue * sprite.getScale().x < 0)
    {
        sprite.setScale(bulletSize * scaleValue,bulletSize);
    }
    elapsedTime = 0.0f;
    isActive = true;
}

void Bullet::deactivate()
{
    isActive = false;
}

void Bullet::update(const float deltaTime)
{
    if (!isActive)
    {
        return;
    }

    elapsedTime += deltaTime;
    sprite.move(velocity * deltaTime);

    if (elapsedTime > lifeTime)
    {
        isActive = false;  // Деактивируем пулю после истечения времени жизни
    }
}

void Bullet::draw(sf::RenderWindow& window)
{
    if (isActive)
    {
        window.draw(sprite);
    }
}

bool Bullet::getActive() const
{
    return isActive;
}

bool Bullet::isOffScreen() const
{
    return false;
}
