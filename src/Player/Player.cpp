#include "Player.h"
#include "ShieldAbility.h"
#include "RollAbility.h"
#include <functional>
#include <string>
#include <cmath>

const int STATES_FRAMES[5]{4,6,2,6,4}; // Количество кадров в анимациях (idle,walking,damage,rolling)
const float PLAYER_HEIGHT = 3.0f;
const float FRAME_TIME = 0.1f;
const float PLAYER_BULLET_HEIGHT = 1.5f;

const std::unordered_map<sf::Keyboard::Key, sf::Vector2f> keyToDirection{
        {sf::Keyboard::A, {-1, 0}},  // Влево
        {sf::Keyboard::D, {1, 0}},   // Вправо
        {sf::Keyboard::W, {0, -1}},  // Вверх
        {sf::Keyboard::S, {0, 1}}    // Вниз
    };    // Hash-таблица для сопоставления клавиш с направлениями

Player::~Player()
{

}

Player::Player()
{
    health = 10;
    speed = 0.15f;

    initAnimations();
    

    elapsedTime = 0.0f;
    currentFrame = 0;
    previousState = IDLE; // Начальное состояние

    sprite.setTexture(animations[IDLE].texture);
    sprite.setTextureRect(animations[IDLE].frames[0]);

    playerBounds = sprite.getLocalBounds(); // Берём размеры спрайта
    sprite.setOrigin(playerBounds.width / 2.0f, playerBounds.height / 2.0f); // Устанавливаем точку, откуда будет рассчитываться положение спрайта

    const std::string playerBulletPath = "E:\\NuclearProject\\icons\\Bullets\\PopGunBullet.png";
    weapons.push_back(std::make_unique<Weapon>(playerBulletPath,PLAYER_BULLET_HEIGHT));
    currentWeapon = weapons[0].get();


    abilities.push_back(std::make_unique<RollAbility>(1.0f));
    abilities.push_back(std::make_unique<ShieldAbility>(3.0f));
    currentAbility = abilities[0].get();

    sprite.setScale(PLAYER_HEIGHT, PLAYER_HEIGHT);  // Увеличиваем спрайт в два раза
}

void Player::initAnimations() {
    const int FRAME_WIDTH = 24;
    const int FRAME_HEIGHT = 24;
    const int GAP = 4;

    const int SHIELD_WIDTH = 39;
    const int SHIELD_HEIGHT = 64;
    const int SHIELD_GAP = 25;

    loadAnimation(IDLE, "E:\\NuclearProject\\icons\\Player\\fishStand.png", STATES_FRAMES[0], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(WALKING, "E:\\NuclearProject\\icons\\Player\\fishWalk.png", STATES_FRAMES[1], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(HURT, "E:\\NuclearProject\\icons\\Player\\fishDamage.png", STATES_FRAMES[2], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(ROLLING, "E:\\NuclearProject\\icons\\Player\\fishRoll.png", STATES_FRAMES[3], FRAME_WIDTH, FRAME_HEIGHT, GAP);
    loadAnimation(SHIELD, "E:\\NuclearProject\\icons\\Player\\fishShield.png", STATES_FRAMES[4], SHIELD_WIDTH, SHIELD_HEIGHT, SHIELD_GAP);
}

void Player::loadAnimation(State state, const std::string& texturePath, int frameCount, int frameWidth, int frameHeight, int gap)
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


Player& Player::getInstance()
{
    static Player player;
    return player;
}


void Player::handleInput(Player& player,const sf::Vector2f& cursorPosition,const float deltaTime)
{
    sf::Vector2f direction(0.f, 0.f);
    bool isWalking = false;

    for (const auto& [key, vec] : keyToDirection)
    {
        if (sf::Keyboard::isKeyPressed(key))
        {
            direction += vec;
            isWalking = true;
        }
    }

    // Проверяем движение и состояние одновременно
    if (!isSpecialActive)
    {
        state = isWalking ? WALKING : IDLE;
    }

    // Перемещаем игрока
    if (direction != sf::Vector2f(0.f, 0.f))
    {
        sprite.move(speed * direction.x, speed * direction.y);
    }

    // Обрабатываем стрельбу
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        sf::Vector2f fireDirection = cursorPosition - sprite.getPosition();
        currentWeapon->fire(sprite.getPosition(), fireDirection,signRotationValue);
    }

    // Обновляем позицию оружия
    sf::Vector2f weaponPos = sprite.getPosition();
    weaponPos.x -= 10.0f * signRotationValue;
    weaponPos.y += 15.0f;
    currentWeapon->update(weaponPos, cursorPosition, deltaTime, signRotationValue);

    // Обрабатываем способность
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        switchAbility();
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        currentAbility->activate(player);
    }

    currentAbility->update(player, deltaTime);
}

void Player::switchAbility()
{
    currentAbilityIndex = (currentAbilityIndex + 1) % abilities.size();
    currentAbility  = abilities[currentAbilityIndex].get();
}

void Player::update(const float deltaTime, const sf::Vector2f& cursorPosition)
{
    elapsedTime += deltaTime;
    const sf::Vector2f& spritePosition = sprite.getPosition();

    // Оптимизация расчета направления взгляда
    if ((cursorPosition.x < spritePosition.x && signRotationValue != 1) ||
        (cursorPosition.x >= spritePosition.x && signRotationValue != -1))
    {
        signRotationValue = (cursorPosition.x < spritePosition.x) ? 1 : -1;
        float scale = (signRotationValue == 1) ? -PLAYER_HEIGHT : PLAYER_HEIGHT;
        sprite.setScale(scale, PLAYER_HEIGHT);

        if (signRotationValue == 1)
        {
            currentWeapon->mirroredLeft();
        }
        else
        {
            currentWeapon->mirroredRight();
        }
    }

    // Оптимизация смены состояний
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
        const Animation& animState = animations[state];
        sprite.setTextureRect(animState.frames[currentFrame]);

        // Обновляем кадр
        if (state == SHIELD) 
        {
            // Если состояние "щит", то кадры проходят только вперед
            if (currentFrame < animState.frameCount - 1)
                currentFrame++;
        } 
        else 
        {
            // Для остальных состояний кадры зацикливаются
            currentFrame = (currentFrame + 1) % animState.frameCount;
        }

        // Если требуется зеркалирование, изменить origin спрайта
        if (state == SHIELD)
        {
            const sf::FloatRect& localBounds = sprite.getLocalBounds();
            sprite.setOrigin(localBounds.width / 2.0f,localBounds.height / 2.0f);
        }
    }
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
    currentWeapon->draw(window);
}

void Player::setHealth(int hp)
{
    health = hp;
}

int Player::getHealth() const
{
    return health;
}

void Player::setSpeed(float spd)
{
    speed = spd;
}

float Player::getSpeed() const
{
    return speed;
}

void Player::setState(State state)
{
    this->state = state;
}

State Player::getState() const
{
    return state;
}

void Player::setSpecialActive(bool isActive)
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

int Player::getScaleValue() const
{
    return signRotationValue;
}