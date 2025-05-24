// Hero.cpp
#include "Hero.h"
#include <iostream>
#include <limits>

// —— Constructors —— 

Hero::Hero(const std::string& name)
    : name_(name)
    , xp_(0)
    , level_(1)
    , hp_(25)
    , maxhp_(25)
    , strength_(4)
    , statPoints_(0)
    , gold_(0)
    , equippedIndex_(-1)
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
    , equippedIndex_(-1)
{}

// —— Getters —— 

const std::string& Hero::getName() const   { return name_; }
int Hero::getXp() const                   { return xp_; }
int Hero::getLevel() const                { return level_; }
int Hero::getHp() const                   { return hp_; }
int Hero::getMaxHp() const                { return maxhp_; }
int Hero::getStrength() const             { return strength_; }
int Hero::getStatPoints() const           { return statPoints_; }
int Hero::getGold() const                 { return gold_; }

// —— Combat —— 

int Hero::attack() {
    int totalDamage = strength_;  // start med helte-styrke

    if (equippedIndex_ >= 0 && equippedIndex_ < (int)weapons_.size()) {
        Weapon &w = weapons_[equippedIndex_];
        int weaponBonus = w.damage + static_cast<int>(strength_ * w.strengthModifier);
        totalDamage += weaponBonus;

        // Vis angreb og reducer durability
        std::cout << name_ << " angriber med " << w.name
                  << " (+" << weaponBonus << " skade). ";
        w.durability--;
        std::cout << "Holdbarhed tilbage: " << w.durability << "\n";

        // Hvis holdbarheden er opbrugt, fjern våbnet
        if (w.durability <= 0) {
            std::cout << w.name << " er gået i stykker!\n";
            weapons_.erase(weapons_.begin() + equippedIndex_);
            equippedIndex_ = -1;
        }
    } else {
        std::cout << name_ << " angriber uden våben (" 
                  << strength_ << " skade).\n";
    }

    return totalDamage;
}

void Hero::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

void Hero::recordKill() {
    if (equippedIndex_ >= 0 && equippedIndex_ < (int)weapons_.size()) {
        weapons_[equippedIndex_].kills++;
        std::cout << name_ << " dræbte et monster med “"
                  << weapons_[equippedIndex_].name << "” "
                  << "(total kills: " << weapons_[equippedIndex_].kills << ")\n";
    }
}

// —— XP & Leveling —— 

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

// —— Stat-allokering —— 

bool Hero::allocateStats(int hpPoints, int strengthPoints) {
    if (hpPoints < 0 || strengthPoints < 0) return false;
    if (hpPoints + strengthPoints > statPoints_) return false;

    maxhp_    += hpPoints;
    strength_ += strengthPoints;
    statPoints_ -= (hpPoints + strengthPoints);

    if (hp_ > maxhp_) hp_ = maxhp_;
    return true;
}

void Hero::restoreHp() {
    hp_ = maxhp_;
    xp_ -= 100;
}

void Hero::setMaxHp(int maxhp) {
    maxhp_ = maxhp;
    if (hp_ > maxhp_) hp_ = maxhp_;
}

// —— Guld —— 

void Hero::addGold(int amount) {
    gold_ += amount;
    std::cout << name_ << " har nu " << gold_ << " guld.\n";
}

// —— Inventory & equipment —— 

void Hero::addWeapon(const std::string& name,
                     int damage,
                     double strengthModifier,
                     int durability)
{
    weapons_.push_back({ name, damage, strengthModifier, durability, 0 });
    std::cout << name_ << " købte “" << name
              << "” (+" << damage << " base skade, x"
              << strengthModifier << " modifier, "
              << durability << " holdbarhed).\n";
    // hvis ingen våben er udstyret, udstyr det automatisk
    if (equippedIndex_ == -1) {
        equippedIndex_ = (int)weapons_.size() - 1;
        std::cout << name_ << " udstyrer automatisk “" << name << "”.\n";
    }
}

bool Hero::equipWeapon(int index) {
    if (index < 0 || index >= (int)weapons_.size()) {
        std::cout << "Ugyldigt våbenvalg.\n";
        return false;
    }
    equippedIndex_ = index;
    auto &w = weapons_[index];
    std::cout << name_ << " udstyrer “" << w.name << "” ("
              << w.durability << " holdbarhed tilbage).\n";
    return true;
}

void Hero::showWeapons() {
    if (weapons_.empty()) {
        std::cout << name_ << " har ingen våben.\n";
        return;
    }
    std::cout << name_ << "s våben:\n";
    for (int i = 0; i < (int)weapons_.size(); ++i) {
        const auto& w = weapons_[i];
        std::cout << " " << (i+1) << ". " << w.name
                  << " (+" << w.damage << " skade, x"
                  << w.strengthModifier << " mod., "
                  << w.durability << " holdbarhed, "
                  << w.kills << " kills)";
        if (i == equippedIndex_) std::cout << " [udstyret]";
        std::cout << "\n";
    }
    std::cout << "Vælg våben at udstyre (1-" << weapons_.size() << "), eller 0 for tilbage: ";
    int choice;
    while (!(std::cin >> choice) || choice < 0 || choice > (int)weapons_.size()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ugyldigt valg (0-" << weapons_.size() << "): ";
    }
    if (choice > 0) {
        equipWeapon(choice - 1);
    }
}
