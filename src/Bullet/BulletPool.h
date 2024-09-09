#ifndef BULLETPOOL_H
#define BULLETPOOL_H

#include "Bullet.h"
#include <vector>
#include <memory>

class BulletPool {
public:
    BulletPool(size_t poolSize, const sf::Texture& bulletTexture,const float bulletsize);

    Bullet* getBullet();
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    std::vector<std::unique_ptr<Bullet>> bullets;
};

#endif // BULLETPOOL_H
