#ifndef WEAPON_H
#define WEAPON_H

#include <SFML/Graphics.hpp>
#include "Bullet/BulletPool.h"

class BulletPool;

class Weapon 
{
public:
    Weapon(const std::string& bulletTexturePath,const float bulletSize);
    void update(const sf::Vector2f& playerPosition,const sf::Vector2f& cursorPosition,const float deltaTime,const int scaleValue);
    void draw(sf::RenderWindow& window);
    void fire(const sf::Vector2f& playerPosition, const sf::Vector2f& direction,const int scaleValue);
    void setTexture(const std::string& bulletTexturePath);
    void setDamage(const int damage);
    void setWeaponReloadSpeed(const int reloadSpeed);
    void setTimeSinceLastShot(const float time);
    void setPosition(const sf::Vector2f& position);
    void mirroredLeft();
    void mirroredRight();
    sf::Sprite getWeaponSprite() const;
    int getWeaponDamage() const;
    float getWeaponReloadSpeed() const;
    float getTimeSinceLastShot() const;
private:

    int weaponDamage;
    float weaponReloadSpeed;
    float timeSinceLastShot;
    float animationAngle;

    const sf::FloatRect& localBounds = weaponSprite.getLocalBounds();
    float fireAngle;
    float fireXCoordinate;
    float fireYCoordinate; 

    std::unique_ptr<BulletPool> bulletPool;

    sf::Sprite weaponSprite;
    sf::Texture weaponTextureLeft;
    sf::Texture weaponTextureRight;

    sf::Texture bulletTexture;

    std::vector<Bullet> bullets; // Вектор для хранения пуль
};
#endif