#include "ShieldAbility.h"
#include <string>

constexpr float SHIELD_SPEED = 0.0000000f;
constexpr float SHIELD_DURATION = 1.25f;

ShieldAbility::ShieldAbility(const float cooldown,const float playerSpeed)
{
    shieldCooldown = cooldown;
    timeSinceActivated = 0.0f;
    this->playerSpeed = playerSpeed;

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

void ShieldAbility::update(Player& player, const float deltaTime)
{
    timeSinceActivated += deltaTime;
    if (player.getSpecialActive() && timeSinceActivated >= SHIELD_DURATION)
    {
        timeSinceActivated = 0.0f;
        player.setState(IDLE);
        player.setSpeed(playerSpeed);
        player.setNormalBounds();
        player.setSpecialActive(false);
    }
}
void ShieldAbility::setTimeSinceActivated(const float time)
{
    timeSinceActivated = time;
}
