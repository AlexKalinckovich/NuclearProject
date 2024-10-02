#include "Enemy/Enemy.h"
#include "Map.h";
#include <cmath>
#include <Player.h>
#include <random>


constexpr float ENEMY_HEIGHT = 3.0f;
constexpr float ENEMY_BULLET_HEIGHT = 2.5f;
constexpr int STATES_FRAMES[3]{6,3,6};
constexpr float DAMAGE_DURATION = 0.5f;
// Конструктор Enemy
Enemy::Enemy(const sf::Vector2f& position)
{
    spriteInit(position);
    this->map = &Map::getInstance();
    // Инициализация оружия
    constexpr BulletOwner owner = ENEMY_BULLET;
    const std::string enemyBulletPath = R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Bullets\EnemyBullet.png)";
    weapon = std::make_unique<Weapon>(enemyBulletPath, ENEMY_BULLET_HEIGHT,owner);
    velocity = sf::Vector2f(0.0f, 0.0f);
    currentFrame = 0;
    state = ENEMY_WALKING;
}

void Enemy::spriteInit(const sf::Vector2f& position)
{
    constexpr int GAP = 8;
    constexpr int WIDTH =  17;
    constexpr int HEIGHT = 17;
    // Загружаем анимацию
    loadAnimation(ENEMY_WALKING,R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Enemy\enemy.png)" , STATES_FRAMES[ENEMY_WALKING], WIDTH, HEIGHT, GAP);
    loadAnimation(ENEMY_HURT,R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Enemy\enemyHurt.png)" , STATES_FRAMES[ENEMY_HURT], WIDTH, HEIGHT, GAP);
    loadAnimation(ENEMY_DEAD,R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Enemy\enemyDead.png)" , STATES_FRAMES[ENEMY_DEAD], WIDTH, HEIGHT, GAP);
    sprite.setTexture(animations[ENEMY_WALKING].texture);
    sprite.setTextureRect(animations[ENEMY_WALKING].frames[0]);
    // Задаем масштаб спрайта
    sprite.setScale(ENEMY_HEIGHT, ENEMY_HEIGHT);


    // Устанавливаем позицию спрайта и его origin
    sprite.setPosition(position);

    sf::FloatRect bounds = sprite.getLocalBounds();
    bounds.width = bounds.width / 2.0f;
    bounds.height = bounds.height / 2.0f + 5;
    sprite.setOrigin(bounds.width,bounds.height);
}

void Enemy::takeDamage(Bullet* bullet)
{
    health -= 1;
    if(health <= 0)
    {
        state = ENEMY_DEAD;
        deactivateBullets();
    }
    else
    {
        currentFrame = 0;
        elapsedTime = frameTime;
        damageAnimationTime = DAMAGE_DURATION;
        state = ENEMY_HURT;
        bullet->setOwner(ENEMY_BULLET);
        bullet->deactivate();
    }
}

void Enemy::deactivateBullets() const
{
    for(const auto& bullet : weapon->getBulletPool()->getBullets())
    {
        bullet->deactivate();
    }
}


void Enemy::loadAnimation(const enemyState state, const std::string& texturePath, const int frameCount, const int frameWidth,const int frameHeight, const int gap)
{
    enemyAnimation animation;
    sf::Image animIcon;
    if (!animIcon.loadFromFile(texturePath))
    {
        return;
    }

    animation.texture.loadFromImage(animIcon);

    for (int i = 0; i < frameCount; ++i)
    {
        animation.frames.emplace_back(i * (frameWidth + gap), 0, frameWidth, frameHeight);
    }

    animation.frameCount = frameCount;
    animations[state] = animation;
}

// Обновление состояния Enemy
void Enemy::update(const Player &player, const float deltaTime)
{
    if(!active) return;

    elapsedTime += deltaTime;
    const sf::Vector2f playerPosition = player.getPosition();
    activeUpdate(playerPosition,deltaTime,player.isAlive());
}

void Enemy::activeUpdate(const sf::Vector2f &playerPosition,const float deltaTime,const bool isPlayerAlive)
{
    const sf::Vector2f& position = sprite.getPosition();
    handleEnemyDamage(deltaTime);
    // Определение направления относительно позиции игрока
    handleRotation(playerPosition,position);
    // Обновление анимации
    handleState();
    // Обновление позиции и логики движения
    handlePlayerMovement(playerPosition,position,deltaTime,isPlayerAlive);
    weapon->update(position, playerPosition, deltaTime, signRotationValue);
}

void Enemy::handleRotation(const sf::Vector2f &playerPosition,const sf::Vector2f& position)
{
    if ((playerPosition.x < position.x && signRotationValue != 1) ||
    (playerPosition.x >= position.x && signRotationValue != -1))
    {
        signRotationValue = playerPosition.x < position.x ? 1 : -1;
        const float scale = signRotationValue >= 1 ? -ENEMY_HEIGHT : ENEMY_HEIGHT;
        sprite.setScale(scale, ENEMY_HEIGHT);

        if (signRotationValue >= 1)
        {
            weapon->mirroredLeft();
        }
        else
        {
            weapon->mirroredRight();
        }
    }
}

void Enemy::handleState()
{
    if (state != previousState)
    {
        currentFrame = 0;
        sprite.setTexture(animations[state].texture);
        previousState = state;
    }

    // Обновление анимации
    if (elapsedTime >= frameTime)
    {
        elapsedTime = 0.0f;
        sprite.setTextureRect(animations[state].frames[currentFrame]);
        currentFrame = (currentFrame + 1) % animations[state].frameCount;
        if(currentFrame == 0 && state == ENEMY_DEAD)
        {
            active = false;
        }
    }
}

void Enemy::handleEnemyDamage(const float deltaTime)
{
    if (state == ENEMY_HURT) {
        damageAnimationTime -= deltaTime;
        // Откидываем назад игрока
        sf::Vector2f knockback = signRotationValue >= 1 ? sf::Vector2f(50.f, 0.f) : sf::Vector2f(-50.f, 0.f);
        knockback *= deltaTime;
        if(!map->isWallAtPosition(sprite.getPosition() + knockback))
        {
            sprite.move(knockback * deltaTime);
        }
        if(damageAnimationTime <= 0)
        {
            state = ENEMY_WALKING;
        }
    }
}

void Enemy::handlePlayerMovement(const sf::Vector2f &playerPosition, const sf::Vector2f &position, const float deltaTime,bool isPlayerAlive)
{
    if (isPlayerInDetectionRange(playerPosition) && isPlayerAlive)
    {
        chasePlayer(playerPosition);
        weapon->fire(position, playerPosition - position, signRotationValue);
    }
    else
    {
        randomMovement(deltaTime);
    }
    const sf::Vector2f offset = velocity * deltaTime;
    if(const sf::Vector2f nextPos = sprite.getPosition() + offset; !map->isWallAtPosition(nextPos))
    {
        sprite.move(offset);
    }
}


// Отрисовка Enemy
void Enemy::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    if(active)
    {
        weapon->draw(window);
    }
}

// Проверка, находится ли игрок в радиусе обнаружения
bool Enemy::isPlayerInDetectionRange(const sf::Vector2f& playerPosition) const
{
    const float x = playerPosition.x - sprite.getPosition().x;
    const float y = playerPosition.y - sprite.getPosition().y;
    const auto distance = std::sqrt(x * x + y * y);
    return distance <= detectionRadius;
}

// Движение к игроку
void Enemy::chasePlayer(const sf::Vector2f &playerPosition)
{
    const sf::Vector2f& direction = playerPosition - sprite.getPosition();

    if (const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y); length < 200.0f) {
        // Отступление, если слишком близко
        velocity = -(direction / length * 100.0f);  // Отходим от игрока
    } else {
        // Двигаемся к игроку
        velocity = direction / length * 100.0f;
    }
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

Weapon *Enemy::getWeapon() const
{
    return weapon.get();
}

sf::Sprite Enemy::getSprite() const
{
    return sprite;
}

