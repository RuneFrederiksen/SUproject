#include "Hero.h"
#include <iostream>

Hero::Hero(const std::string& name)
    : name_(name)
    , xp_(0)
    , level_(1)
    , hp_(25)
    , maxhp_(25)
    , strength_(4)
    , statPoints_(0)
    , gold_(0)               
{}

Hero::Hero(const std::string& name,
           int xp,
           int level,
           int hp,
           int maxhp,
           int strength,
           int statPoints)
    : name_(name)
    , xp_(xp)
    , level_(level)
    , hp_(hp)
    , maxhp_(maxhp)
    , strength_(strength)
    , statPoints_(statPoints)
    , gold_(0)               
{}

const std::string& Hero::getName() const { return name_; }
int Hero::getXp() const            { return xp_; }
int Hero::getLevel() const         { return level_; }
int Hero::getHp() const            { return hp_; }
int Hero::getMaxHp() const         { return maxhp_; }
int Hero::getStrength() const      { return strength_; }
int Hero::getStatPoints() const    { return statPoints_; }

int Hero::getGold() const          { return gold_; } 

void Hero::addGold(int amount) {                  
    gold_ += amount;
    std::cout << name_ << " har nu " << gold_ << " guld.\n";
}

int Hero::attack() const { return strength_; }

void Hero::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

int Hero::xpThreshold() const {
    return level_ * 1000;
}

void Hero::addXp(int amount) {
    xp_ += amount;
    if (xp_ < 0) xp_ = 0;
    std::cout << name_ << " har nu " << xp_ << " XP.\n";
    if (canLevelUp()) {
        std::cout << name_ << " kan level up!\n";
    }
}

bool Hero::canLevelUp() const {
    return xp_ >= xpThreshold();
}

bool Hero::levelUp() {
    if (!canLevelUp()) return false;
    xp_ -= xpThreshold();
    level_++;
    statPoints_ += 5;
    return true;
}

bool Hero::allocateStats(int hpPoints, int strengthPoints) {
    if (hpPoints < 0 || strengthPoints < 0) return false;
    if (hpPoints + strengthPoints > statPoints_) return false;

    maxhp_ += hpPoints;
    strength_ += strengthPoints;
    statPoints_ -= (hpPoints + strengthPoints);

    if (hp_ > maxhp_)
        hp_ = maxhp_;
    return true;
}

void Hero::restoreHp() {
    hp_ = maxhp_;
    xp_ -= 100;

}

void Hero::setMaxHp(int maxhp) {
    maxhp_ = maxhp;
    if (hp_ > maxhp_)
        hp_ = maxhp_;
}
