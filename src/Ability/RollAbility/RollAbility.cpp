#include "RollAbility.h"

const float ROLLING_SPEED = 0.35f;
const float NORMAL_SPEED = 0.15f;
const float ROLLING_DURATION = 0.4f;

RollAbility::RollAbility(float cooldown)
{
    rollCooldown = cooldown;
    timeSinceLastRoll = cooldown + 1.0f;
}

void RollAbility::activate(Player& player)
{
    if (timeSinceLastRoll >= rollCooldown)
    {
        player.setState(ROLLING);
        player.setSpeed(ROLLING_SPEED);
        player.setSpecialActive(true);
        timeSinceLastRoll = 0.0f;
    }
}

void RollAbility::update(Player& player, float deltaTime)
{
    timeSinceLastRoll += deltaTime;

    if (player.getSpecialActive() && timeSinceLastRoll >= ROLLING_DURATION)
    {
        player.setState(IDLE);
        player.setSpeed(NORMAL_SPEED);
        player.setSpecialActive(false);
    }
}
