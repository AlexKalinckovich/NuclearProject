#include "Bullet.h"
#include <SFML/Graphics.hpp>
#include <cmath>

constexpr float BULLET_SPEED = 500.0f;
constexpr float BULLET_LIFETIME = 5.0f;

Bullet::Bullet(const sf::Vector2f& position, const sf::Vector2f& direction, const sf::Texture& texture, const float angle,const float bulletSize,const BulletOwner owner)
    : lifeTime(BULLET_LIFETIME), elapsedTime(0.0f), isBulletActive(false)
{
    this->owner = owner;
    map = &Map::getInstance();
    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setScale(bulletSize, bulletSize);
    this->bulletSize = bulletSize;

    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = BULLET_SPEED * direction / length;

    sprite.setRotation(angle);
}

void Bullet::activate(const sf::Vector2f& position, const sf::Vector2f& direction, const float angle,const float scaleValue)
{
    sprite.setPosition(position);
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = BULLET_SPEED * direction / length;
    sprite.setRotation(angle);
    if(scaleValue * sprite.getScale().x < 0)
    {
        sprite.setScale(bulletSize * scaleValue,bulletSize);
    }
    elapsedTime = 0.0f;
    isBulletActive = true;
}

void Bullet::deactivate()
{
    isBulletActive = false;
}

void Bullet::update(const float deltaTime)
{
    if (!isBulletActive)
    {
        return;
    }

    elapsedTime += deltaTime;

    const sf::Vector2f offset = velocity * deltaTime;
    const sf::Vector2f nextPos = sprite.getPosition() + offset;
    sprite.move(offset);

    if (elapsedTime > lifeTime || map->isWallAtPosition(nextPos))
    {
        isBulletActive = false;  // Деактивируем пулю после истечения времени жизни
    }
}

void Bullet::draw(sf::RenderWindow& window) const
{
    if (isBulletActive)
    {
        window.draw(sprite);
    }
}

bool Bullet::isActive() const
{
    return isBulletActive;
}

bool Bullet::isOffScreen()
{
    return false;
}

bool Bullet::checkCollision(const sf::FloatRect& targetBounds) const
{
    const sf::Vector2f position = sprite.getPosition();
    // Проверка столкновения пули с целью
    const sf::FloatRect bulletBounds(position.x, position.y, 10.0f, 10.0f);  // Примерные размеры пули
    return bulletBounds.intersects(targetBounds);
}

void Bullet::reflect(const BulletOwner newOwner)
{
    // Инвертируем направление пули (меняем направление полета)
    velocity = -velocity;

    // Меняем владельца на игрока
    owner = newOwner;
}

BulletOwner Bullet::getOwner() const
{
    return owner;
}

void Bullet::setOwner(const BulletOwner oldOwner)
{
    owner = oldOwner;
}
