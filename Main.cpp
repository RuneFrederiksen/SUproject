// Hero.cpp
#include "Hero.h"

// Constructor for a new Hero with default stats
Hero::Hero(const std::string& name)
    : name_(name), xp_(0), level_(1), hp_(10), strength_(2), statPoints_(0) {}

// Constructor for loading a saved Hero
Hero::Hero(const std::string& name, int xp, int level, int hp, int strength, int statPoints)
    : name_(name), xp_(xp), level_(level), hp_(hp), strength_(strength), statPoints_(statPoints) {}

// Getters
const std::string& Hero::getName() const { return name_; }
int Hero::getXp() const { return xp_; }
int Hero::getLevel() const { return level_; }
int Hero::getHp() const { return hp_; }
int Hero::getStrength() const { return strength_; }
int Hero::getStatPoints() const { return statPoints_; }

// Returnerer skaden hero laver
int Hero::attack() const { return strength_; }

// Påfører skade på hero, reducerer hp (gulv ved 0)
void Hero::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

// Intern XP‐grænse baseret på nuværende level
int Hero::xpThreshold() const {
    return level_ * 1000;
}

// Tilføj XP (kan være negativt); XP vil aldrig gå under 0
void Hero::addXp(int amount) {
    xp_ += amount;
    if (xp_ < 0) {
        xp_ = 0;
    }
}

// Tjek om hero kan level‐up
bool Hero::canLevelUp() const {
    return xp_ >= xpThreshold();
}

// Udfør level‐up hvis muligt; returnerer true ved succes
bool Hero::levelUp() {
    if (!canLevelUp()) return false;

    xp_ -= xpThreshold();
    level_++;
    statPoints_ += 3;    // Tildel 3 stat‐points
    return true;
}

// Fordel stat‐points: summen af hpPoints og strengthPoints ≤ statPoints_
bool Hero::allocateStats(int hpPoints, int strengthPoints) {
    if (hpPoints < 0 || strengthPoints < 0) return false;
    if (hpPoints + strengthPoints > statPoints_) return false;

    hp_ += hpPoints;
    strength_ += strengthPoints;
    statPoints_ -= (hpPoints + strengthPoints);
    return true;
}
