#include "BulletPool.h"

BulletPool::BulletPool(const size_t poolSize, const sf::Texture& bulletTexture,const float bulletSize)
{
    bullets.reserve(poolSize);
    for (size_t i = 0; i < poolSize; i++)
    {
        bullets.push_back(std::make_unique<Bullet>(sf::Vector2f(-1000, -1000), sf::Vector2f(0, 0), bulletTexture, 0.0f,bulletSize));
    }
}

Bullet* BulletPool::getBullet() const
{
    for (auto& bullet : bullets)
    {
        if (!bullet->getActive())
        {  // Проверяем, активна ли пуля
            return bullet.get();    // Возвращаем неактивную пулю
        }
    }
    return nullptr;  // Все пули заняты
}

void BulletPool::update(const float deltaTime)
{
    for (const auto& bullet : bullets)
    {
        if (bullet->getActive())
        {
            bullet->update(deltaTime);
        }
    }
}

void BulletPool::draw(sf::RenderWindow& window) const
{
    for (const auto& bullet : bullets)
    {
        if (bullet->getActive())
        {
            bullet->draw(window);
        }
    }
}
