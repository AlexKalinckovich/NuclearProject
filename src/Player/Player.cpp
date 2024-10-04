#include "Player.h"
#include "ShieldAbility.h"
#include "RollAbility.h"
#include <functional>
#include <string>

constexpr int STATES_FRAMES[6]{4,6,4,6,4,6}; // Количество кадров в анимациях (idle,walking,damage,rolling)
constexpr float PLAYER_HEIGHT = 3.0f;
constexpr float FRAME_TIME = 0.1f;
constexpr float PLAYER_BULLET_HEIGHT = 1.5f;
constexpr float PLAYER_SPEED = 3.0f;
constexpr float DAMAGE_DURATION = 0.5f;
const std::unordered_map<sf::Keyboard::Key, sf::Vector2f> keyToDirection{
        {sf::Keyboard::A, {-1, 0}},  // Влево
        {sf::Keyboard::D, {1, 0}},   // Вправо
        {sf::Keyboard::W, {0, -1}},  // Вверх
        {sf::Keyboard::S, {0, 1}}    // Вниз
    };    // Hash-таблица для сопоставления клавиш с направлениями

Player::~Player()
= default;


Player::Player()
{
    health = 10;
    maxHealth = 10;
    speed = PLAYER_SPEED;

    initAnimations();

    this->map = &Map::getInstance();
    elapsedTime = 0.0f;
    currentFrame = 0;
    previousState = IDLE; // Начальное состояние

    sprite.setTexture(animations[IDLE].texture);
    sprite.setTextureRect(animations[IDLE].frames[0]);

    playerBounds = sprite.getLocalBounds(); // Берём размеры спрайта
    sprite.setOrigin(playerBounds.width / 2.0f, playerBounds.height / 2.0f); // Устанавливаем точку, откуда будет рассчитываться положение спрайта

    const std::string playerBulletPath = R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Bullets\PopGunBullet.png)";
    constexpr BulletOwner owner = PLAYER_BULLET;
    weapons.push_back(std::make_unique<Weapon>(playerBulletPath,PLAYER_BULLET_HEIGHT,owner));
    currentWeapon = weapons[0].get();


    abilities.push_back(std::make_unique<RollAbility>(1.0f,PLAYER_SPEED));
    abilities.push_back(std::make_unique<ShieldAbility>(1.5f,PLAYER_SPEED));
    currentAbility = abilities[0].get();

    sprite.setScale(PLAYER_HEIGHT, PLAYER_HEIGHT);  // Увеличиваем спрайт в два раза
}

void Player::resetPlayer()
{
    state = IDLE;
    active = true;
    health = maxHealth;
    isSpinning = false;
    sprite.setRotation(0);
}

void Player::initAnimations() {
    constexpr int FRAME_WIDTH = 24;
    constexpr int FRAME_HEIGHT = 24;
    constexpr int GAP = 4;

    constexpr int SHIELD_WIDTH = 39;
    constexpr int SHIELD_HEIGHT = 64;
    constexpr int SHIELD_GAP = 25;

    loadAnimation(IDLE, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishStand.png)", STATES_FRAMES[IDLE], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(WALKING, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishWalk.png)", STATES_FRAMES[WALKING], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(HURT, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishDamage.png)", STATES_FRAMES[HURT], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(ROLLING, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishRoll.png)", STATES_FRAMES[ROLLING], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(DEAD, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishDead.png)", STATES_FRAMES[DEAD], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(SHIELD, R"(C:\Users\brota\CLionProjects\NuclearProject\icons\Player\fishShield.png)", STATES_FRAMES[SHIELD], SHIELD_WIDTH, SHIELD_HEIGHT, SHIELD_GAP);
}

void Player::loadAnimation(const State state, const std::string& texturePath, const int frameCount, const int frameWidth, const int frameHeight, const int gap)
{
    Animation animation;
    sf::Image animIcon;
    if (!animIcon.loadFromFile(texturePath))
    {
        return;
    }

    animIcon.createMaskFromColor(sf::Color(255,0,255));
    animation.texture.loadFromImage(animIcon);

    for (int i = 0; i < frameCount; ++i)
    {
        animation.frames.emplace_back(i * (frameWidth + gap), 0, frameWidth, frameHeight);
    }

    animation.frameCount = frameCount;
    animations[state] = animation;
}

void Player::startSpinning() {
    isSpinning = true;  // Флаг, что игрок начал вращаться
}


Player& Player::getInstance()
{
    static Player player;
    return player;
}

void Player::handleInput(Player& player, const sf::Vector2f& cursorPosition, const float deltaTime)
{
    if(state != DEAD)
    {
        processMovement();
        processShooting(cursorPosition,deltaTime);
        processAbilities(player, deltaTime);
    }
}

void Player::processMovement() {
    sf::Vector2f direction(0.f, 0.f);
    bool isWalking = false;

    for (const auto& [key, vec] : keyToDirection) {
        if (sf::Keyboard::isKeyPressed(key)) {
            direction += vec;
            isWalking = true;
        }
    }

    if (!isSpecialActive && state != HURT && state != DEAD)
    {
        state = isWalking ? WALKING : IDLE;
    }


    // Перемещаем игрока
    if (direction != sf::Vector2f(0.f, 0.f))
    {
        // Рассчитываем следующую позицию игрока
        const sf::Vector2f offset = speed * direction;
        const sf::Vector2f nextPos = sprite.getPosition() + offset;
        // Проверяем, нет ли стены на этой позиции
        if (!map->isWallAtPosition(nextPos))
        {
            // Если стены нет, перемещаем игрока
            sprite.move(offset);
        }
    }
}

void Player::processShooting(const sf::Vector2f& cursorPosition,const float deltaTime) const
{
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        const sf::Vector2f fireDirection = cursorPosition - sprite.getPosition();
        currentWeapon->fire(sprite.getPosition(), fireDirection, signRotationValue);
    }
    // Обновляем позицию оружия
    sf::Vector2f weaponPos = sprite.getPosition();
    weaponPos.x -= 10.0f * signRotationValue;
    weaponPos.y += 15.0f;
    currentWeapon->update(weaponPos, cursorPosition, deltaTime, signRotationValue);
}

void Player::processAbilities(Player& player, const float deltaTime) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && !abilitySwitching && !isSpecialActive) {
        switchAbility();
        abilitySwitching = true;
    } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        abilitySwitching = false;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        currentAbility->activate(player);
    }

    currentAbility->update(player, deltaTime);
}


void Player::switchAbility()
{
    currentAbilityIndex = (currentAbilityIndex + 1) % abilities.size();
    currentAbility->setTimeSinceActivated(INT_MAX);
    currentAbility  = abilities[currentAbilityIndex].get();
}

void Player::update(const float deltaTime, const sf::Vector2f& cursorPosition)
{
    if(!active) return;
    elapsedTime += deltaTime;
    if(isSpinning)
    {
        sprite.rotate(360.0f * deltaTime);
    }
    // Если состояние "получение урона"
    handleDamage(deltaTime);
    animationUpdate(cursorPosition);
}

void Player::animationUpdate(const sf::Vector2f &cursorPosition)
{

    // Оптимизация расчета направления взгляда
    handleRotation(cursorPosition);

    // Оптимизация смены состояний
    handleState();
}

void Player::handleRotation(const sf::Vector2f& cursorPosition)
{
    if (const sf::Vector2f& spritePosition = sprite.getPosition(); (cursorPosition.x < spritePosition.x && signRotationValue != 1) ||
                                                                   (cursorPosition.x >= spritePosition.x && signRotationValue != -1))
    {
        signRotationValue = cursorPosition.x < spritePosition.x ? 1 : -1;
        const float scale = signRotationValue >= 1 ? -PLAYER_HEIGHT : PLAYER_HEIGHT;
        sprite.setScale(scale, PLAYER_HEIGHT);

        if (signRotationValue >= 1)
        {
            currentWeapon->mirroredLeft();
        }
        else
        {
            currentWeapon->mirroredRight();
        }
    }
}
void Player::handleDamage(const float deltaTime)
{
    if (state == HURT)
    {
        damageAnimationTime -= deltaTime;
        // Откидываем назад игрока
        sf::Vector2f knockback = (signRotationValue == 1) ? sf::Vector2f(50.f, 0.f) : sf::Vector2f(-50.f, 0.f);
        knockback *= deltaTime;
        if(!map->isWallAtPosition(sprite.getPosition() + knockback))
        {
            sprite.move(knockback * deltaTime);
        }
        if(damageAnimationTime <= 0)
        {
            state = IDLE;
        }
    }
}

void Player::handleState()
{
    if (state != previousState)
    {
        currentFrame = 0;
        sprite.setTexture(animations[state].texture);
        previousState = state;
    }

    // Анимация обновляется только при достижении интервала времени кадра
    if (elapsedTime >= FRAME_TIME)
    {
        elapsedTime = 0.0f;

        // Получаем текущее состояние анимации
        sprite.setTextureRect(animations[state].frames[currentFrame]);

        // Обновляем кадр
        if (state == SHIELD)
        {
            // Если состояние "щит", то кадры проходят только вперед
            if (currentFrame < animations[state].frameCount - 1)
                currentFrame++;
        }
        else
        {
            // Для остальных состояний кадры зацикливаются
            currentFrame = (currentFrame + 1) % animations[state].frameCount;
        }

        // Если требуется зеркалирование, изменить origin спрайта
        if (state == SHIELD)
        {
            const sf::FloatRect& localBounds = sprite.getLocalBounds();
            sprite.setOrigin(localBounds.width / 2.0f,localBounds.height / 2.0f);
        }
        if(state == DEAD && currentFrame == 0)
        {
            active = false;
        }
    }
}

void Player::takeDamage(Bullet* bullet) {
    if(state != ROLLING)
    {
        if(state == SHIELD)
        {
            bullet->reflect(PLAYER_BULLET);
        }
        else
        {
            health -= 1;
            if(health <= 0)
            {
                state = DEAD;
                health = 0;
            }
            else
            {
                state = HURT;  // Переключаем состояние на "получение урона"
                currentFrame = 0;
                elapsedTime = 0;
                damageAnimationTime = DAMAGE_DURATION;
            }
            bullet->deactivate();
        }
    }
}


void Player::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
    if(active)
    {
        currentWeapon->draw(window);
    }
}




void Player::setHealth(const int hp)
{
    health = hp;
}

unsigned Player::getHealth() const
{
    return health;
}

void Player::setSpeed(const float spd)
{
    speed = spd;
}

float Player::getSpeed() const
{
    return speed;
}

void Player::setState(const State state)
{
    this->state = state;
}

State Player::getState() const
{
    return state;
}

void Player::setSpecialActive(const bool isActive)
{
    isSpecialActive = isActive;
}

bool Player::getSpecialActive() const
{
    return isSpecialActive;
}

void Player::setNormalBounds()
{
    sprite.setOrigin(playerBounds.width / 2.0f,playerBounds.height / 2.0f);
}

sf::Vector2f Player::getPosition() const
{
    return sprite.getPosition();
}

void Player::setPosition(const sf::Vector2f& position)
{
    sprite.setPosition(position);
}

float Player::getScaleValue() const
{
    return signRotationValue;
}

Weapon* Player::getWeapon() const
{
    return currentWeapon;
}

sf::Sprite Player::getSprite() const
{
    return sprite;
}
bool Player::isAlive() const
{
    return health > 0;
}
int Player::getMaxHealth() const
{
    return maxHealth;
}
