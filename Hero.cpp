#include "Hero.h"
#include <iostream>

Hero::Hero(const std::string& name)
    : name_(name), xp_(0), level_(1), hp_(10), strength_(2) {}

Hero::Hero(const std::string& name, int xp, int level, int hp, int strength)
    : name_(name), xp_(xp), level_(level), hp_(hp), strength_(strength) {
    if (xp_ < 0 || level_ < 1 || hp_ < 0 || strength_ < 1) {
        std::cerr << "Invalid Hero state!" << std::endl;
        throw std::invalid_argument("Invalid Hero state!");
    }
}

const std::string& Hero::getName() const {
    return name_;
}
int Hero::getXp() const {
    return xp_;
}
int Hero::getLevel() const {
    return level_;
}
int Hero::getHp() const {
    return hp_;
}
int Hero::getStrength() const {
    return strength_;
}
int Hero::attack() const {
    return strength_;
}
void Hero::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) {
        hp_ = 0; // Ensure hp doesn't go negative
    }
}
void Hero::addXp(int amount) {
    xp_ += amount;
    std::cout << name_ << " gained " << amount << " XP!" << std::endl;
    if (xp_ >= level_ * 1000) {
        std::cout << "you can level up!" << std::endl;
    }
    if(xp_ < 0) {
        xp_ = 0; // Ensure xp doesn't go negative
    }
    
}

void Hero::levelUp() {
    level_++;
    hp_ += 5; // Increase hp on level up
    strength_ += 1; // Increase strength on level up
    std::cout << name_ << " leveled up to level " << level_ << "!" << std::endl;
}

