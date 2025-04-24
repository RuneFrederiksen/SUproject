#include "Enemy.h"

Enemy::Enemy(const std::string& name, int hp, int damage, int xpReward)
    : name_(name), hp_(hp), damage_(damage), xpReward_(xpReward) {}


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

int Enemy::attack() const {
    return damage_;
}


void Enemy::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) {
        hp_ = 0;
    }
}

bool Enemy::isAlive() const {
    return hp_ > 0;
}

