// Ability.h
#ifndef ABILITY_H
#define ABILITY_H

#include <SFML/Graphics.hpp>
#include "Player/Player.h"

class Player;

class Ability {
public:
    virtual void activate(Player& player) = 0;
    virtual void update(Player& player, float deltaTime) = 0;
    virtual void setTimeSinceActivated(float time) = 0;
    virtual ~Ability() {}
};

#endif // ABILITY_H
