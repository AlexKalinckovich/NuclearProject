#include "RollAbility.h"

constexpr float ROLLING_DURATION = 0.4f;

RollAbility::RollAbility(const float cooldown,const float playerSpeed)
{
    rollCooldown = cooldown;
    timeSinceLastRoll = cooldown + 1.0f;
    this->playerSpeed = playerSpeed;
    rollingSpeed = playerSpeed * 2;
}

void RollAbility::activate(Player& player)
{
    if (timeSinceLastRoll >= rollCooldown)
    {
        player.setState(ROLLING);
        player.setSpeed(rollingSpeed);
        player.setSpecialActive(true);
        timeSinceLastRoll = 0.0f;
    }
}

void RollAbility::update(Player& player, const float deltaTime)
{
    timeSinceLastRoll += deltaTime;

    if (player.getSpecialActive() && timeSinceLastRoll >= ROLLING_DURATION)
    {
        player.setState(IDLE);
        player.setSpeed(playerSpeed);
        player.setSpecialActive(false);
    }
}
