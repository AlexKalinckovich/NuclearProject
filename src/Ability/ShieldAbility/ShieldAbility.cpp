#include "ShieldAbility.h"
#include <string>

const float NORMAL_SPEED = 0.15f;
const float SHIELD_SPEED = 0.0000000f;
const float SHIELD_DURATION = 1.25f;

ShieldAbility::ShieldAbility(float cooldown)
{
    shieldCooldown = cooldown;
    timeSinceActivated = 0.0f;
}

void ShieldAbility::activate(Player& player)
{
    if (timeSinceActivated >= shieldCooldown)
    {
        player.setState(SHIELD);
        player.setSpeed(SHIELD_SPEED);
        player.setSpecialActive(true);
        timeSinceActivated = 0.0f;
    }
}

void ShieldAbility::update(Player& player, float deltaTime)
{
    timeSinceActivated += deltaTime;
    if (player.getSpecialActive() && timeSinceActivated >= SHIELD_DURATION)
    {
        timeSinceActivated = 0.0f;
        player.setState(IDLE);
        player.setSpeed(NORMAL_SPEED);
        player.setNormalBounds();
        player.setSpecialActive(false);
    }
}