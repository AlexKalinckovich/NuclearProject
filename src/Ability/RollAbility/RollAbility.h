#ifndef ROLL_ABILITY_H
#define ROLL_ABILITY_H
#include "Ability.h"
class RollAbility final : public Ability
{
    public:
        explicit RollAbility(float cooldown,float playerSpeed);
        void activate(Player& player) override;
        void update(Player& player, float deltaTime) override;
        void setTimeSinceActivated(float time) override;
    private:
        float rollCooldown;       // Время перезарядки для переката
        float timeSinceLastRoll;  // Время с момента последнего использования
        float playerSpeed;
        float rollingSpeed;
};

#endif // ROLL_ABILITY_H
