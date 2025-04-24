// Enemy.cpp
#include "Enemy.h"

// Constructor implementation
Enemy::Enemy(const std::string& name, int hp, int damage, int xpReward)
    : name_(name), hp_(hp), damage_(damage), xpReward_(xpReward) {}

// Getters
const std::string& Enemy::getName() const {
    return name_;
}

int Enemy::getHp() const {
    return hp_;
}

int Enemy::getDamage() const {
    return damage_;
}

int Enemy::getXpReward() const {
    return xpReward_;
}

// Returns the damage this enemy deals
int Enemy::attack() const {
    return damage_;
}

// Applies damage to the enemy, reducing hp (floors at 0)
void Enemy::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) {
        hp_ = 0;
    }
}

// Check if enemy is still alive
bool Enemy::isAlive() const {
    return hp_ > 0;
}

