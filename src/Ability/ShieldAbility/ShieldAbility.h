#ifndef SHIELD_ABILITY_H
#define SHIELD_ABILITY_H
#include "Ability.h"

class ShieldAbility : public Ability
{
public:
    ShieldAbility(float cooldown);
    void activate(Player& player) override;
    void update(Player& player, float deltaTime) override;
private:
    float shieldCooldown;     // Длительность способности
    float timeSinceActivated; // Время с момента активации
    bool isHalfDuration = false;
};

#endif