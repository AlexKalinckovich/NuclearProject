#ifndef BULLET_H
#define BULLET_H

#include <Map.h>
#include <SFML/Graphics.hpp>
class Map;
enum BulletOwner { PLAYER_BULLET, ENEMY_BULLET };
class Bullet
{
    public:

        Bullet(const sf::Vector2f& position,const sf::Vector2f& direction,const sf::Texture& texture,float angle,float bulletSize,BulletOwner owner);
        void update(float deltaTime);
        void draw(sf::RenderWindow& window) const;

        static bool isOffScreen();
        bool isActive() const;

        void activate(const sf::Vector2f& position, const sf::Vector2f& direction, float angle, float scaleValue);
        void deactivate();
        void reflect(BulletOwner newOwner);
        bool checkCollision(const sf::FloatRect& targetBounds) const;

        void setOwner(BulletOwner oldOwner);
        BulletOwner getOwner() const;
    private:
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Vector2f velocity;
        Map* map;
        float speed{};
        float lifeTime;
        float elapsedTime;
        float bulletSize;
        bool isBulletActive;
        BulletOwner owner;
};

#endif