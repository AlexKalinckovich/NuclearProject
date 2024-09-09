#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet
{
    public:
        Bullet(const sf::Vector2f& position,const sf::Vector2f& direction,const sf::Texture& texture,const float angle,const float bulletSize);
        void update(const float deltaTime);
        void draw(sf::RenderWindow& window);
        bool isOffScreen() const;
        bool getActive() const;

        void activate(const sf::Vector2f& position, const sf::Vector2f& direction, float angle,const int scaleValue);
        void deactivate();

    private:
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Vector2f velocity;

        float speed;
        float lifeTime;
        float elapsedTime;
        float bulletSize;
        bool isActive;
};

#endif