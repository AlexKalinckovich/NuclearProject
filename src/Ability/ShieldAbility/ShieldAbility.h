#ifndef SHIELD_ABILITY_H
#define SHIELD_ABILITY_H
#include "Ability.h"

class ShieldAbility final : public Ability
{
public:
    ShieldAbility(float cooldown,float playerSpeed);
    void activate(Player& player) override;
    void update(Player& player, float deltaTime) override;
    void setTimeSinceActivated(float time) override;
private:
    float shieldCooldown;     // Длительность способности
    float timeSinceActivated; // Время с момента активации
    bool isHalfDuration = false;
    float playerSpeed;
};

#endif