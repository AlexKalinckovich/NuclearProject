#ifndef BULLETPOOL_H
#define BULLETPOOL_H

#include "Bullet.h"
#include <vector>
#include <memory>
class Bullet;
class BulletPool {
public:

    BulletPool(size_t poolSize, const sf::Texture& bulletTexture,float bulletSize,BulletOwner owner);

    [[nodiscard]] Bullet* getBullet() const;
    void update(float deltaTime);
    void draw(sf::RenderWindow& window) const;
    [[nodiscard]] const std::vector<std::unique_ptr<Bullet> >& getBullets() const;

private:
    std::vector<std::unique_ptr<Bullet>> bullets;
};

#endif // BULLETPOOL_H
