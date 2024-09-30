#include "Enemy/Enemy.h"
#include <cmath>
#include <random>


constexpr int FRAME_COUNT = 6;
constexpr float ENEMY_HEIGHT = 3.0f;
constexpr float ENEMY_BULLET_HEIGHT = 2.5f;

// Конструктор Enemy
Enemy::Enemy(const sf::Vector2f& position)
    : detectionRadius(200.0f), moveTimer(0.0f), active(true) 
{
    // Загружаем текстуру
    const std::string& texturePath = R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Enemy\enemy.png)";
    enemyTexture.loadFromFile(texturePath);
    // Загружаем анимацию
    loadAnimation();

    sprite.setTexture(enemyTexture);
    sprite.setTextureRect(animationFrames[0]);
    // Задаем масштаб спрайта
    sprite.setScale(ENEMY_HEIGHT, ENEMY_HEIGHT);

    // Устанавливаем позицию спрайта и его origin
    sprite.setPosition(position);

    sf::FloatRect bounds = sprite.getLocalBounds();
    bounds.width = bounds.width / 2.0f;
    bounds.height = bounds.height / 2.0f + 5;
    sprite.setOrigin(bounds.width,bounds.height);

    // Инициализация оружия
    const std::string enemyBulletPath = R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Bullets\EnemyBullet.png)";
    weapon = std::make_unique<Weapon>(enemyBulletPath,ENEMY_BULLET_HEIGHT);

    velocity = sf::Vector2f(0.0f, 0.0f);                                                               
    currentFrame = 0;
}

void Enemy::loadAnimation()
{
    for(int i = 0;i < FRAME_COUNT;i++) {
        constexpr int GAP = 8;
        constexpr int WIDTH =  17;
        constexpr int HEIGHT = 17;
        animationFrames.emplace_back(i * (WIDTH + GAP), 0, WIDTH, HEIGHT);
    }
}

// Обновление состояния Enemy
void Enemy::update(const sf::Vector2f &playerPosition, const float deltaTime)
{
    elapsedTime += deltaTime;
    const sf::Vector2f& position = sprite.getPosition();

    // Определение направления относительно позиции игрока
    if ((playerPosition.x < position.x && signRotationValue != 1) ||
        (playerPosition.x >= position.x && signRotationValue != -1))
    {
        signRotationValue = (playerPosition.x < position.x) ? 1 : -1;
        const float scale = signRotationValue == 1 ? -ENEMY_HEIGHT : ENEMY_HEIGHT;
        sprite.setScale(scale, ENEMY_HEIGHT);

        if (signRotationValue == 1)
        {
            weapon->mirroredLeft();
        } 
        else
        {
            weapon->mirroredRight();
        }
    }

    // Обновление анимации
    if (elapsedTime >= frameTime) 
    {
        elapsedTime = 0.0f;
        currentFrame = (currentFrame + 1) % animationFrames.size();
        sprite.setTextureRect(animationFrames[currentFrame]);
    }

    // Обновление позиции и логики движения
    if (isPlayerInDetectionRange(playerPosition)) 
    {
        chasePlayer(playerPosition, deltaTime);
        weapon->fire(position, playerPosition - position, signRotationValue);
    } 
    else 
    {
        randomMovement(deltaTime);
    }

    sprite.move(velocity * deltaTime);
    weapon->update(position, playerPosition, deltaTime, signRotationValue);
}


// Отрисовка Enemy
void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    weapon->draw(window);
}

// Проверка, находится ли игрок в радиусе обнаружения
bool Enemy::isPlayerInDetectionRange(const sf::Vector2f& playerPosition) const
{
    const auto distance = static_cast<float>(std::sqrt(std::pow(playerPosition.x - sprite.getPosition().x, 2) +
                                     std::pow(playerPosition.y - sprite.getPosition().y, 2)));
    return distance <= detectionRadius;
}

// Движение к игроку
void Enemy::chasePlayer(const sf::Vector2f& playerPosition, float deltaTime)
{
    const sf::Vector2f& direction = playerPosition - sprite.getPosition();
    const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    velocity = (direction / length) * 100.0f;  // Задаем скорость в сторону игрока
}

// Генерация случайного направления движения
void Enemy::randomMovement(const float deltaTime) {
    moveTimer += deltaTime;
    if (moveTimer >= 1.0f)
    {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution dist(-1.0f, 1.0f);

        velocity.x = dist(rng) * 50.0f;
        velocity.y = dist(rng) * 50.0f;

        moveTimer = 0.0f;
    }
}

// Установка радиуса обнаружения
void Enemy::setDetectionRadius(const float radius)
{
    detectionRadius = radius;
}

// Проверка на активность Enemy
bool Enemy::isActive() const
{
    return active;
}
