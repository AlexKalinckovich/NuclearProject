#ifndef ROLL_ABILITY_H
#define ROLL_ABILITY_H
#include "Ability.h"
class RollAbility : public Ability
{
    public:
        RollAbility(float cooldown);
        void activate(Player& player) override;
        void update(Player& player, float deltaTime) override;
    private:
        float rollCooldown;       // Время перезарядки для переката
        float timeSinceLastRoll;  // Время с момента последнего использования
};

#endif // ROLL_ABILITY_H
