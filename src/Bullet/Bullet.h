#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet
{
    public:
        Bullet(const sf::Vector2f& position,const sf::Vector2f& direction,const sf::Texture& texture,float angle,float bulletSize);
        void update(float deltaTime);
        void draw(sf::RenderWindow& window) const;

        static bool isOffScreen();
        bool getActive() const;

        void activate(const sf::Vector2f& position, const sf::Vector2f& direction, float angle, float scaleValue);
        void deactivate();

    private:
        sf::Texture texture;
        sf::Sprite sprite;
        sf::Vector2f velocity;

        float speed{};
        float lifeTime;
        float elapsedTime;
        float bulletSize;
        bool isActive;
};

#endif