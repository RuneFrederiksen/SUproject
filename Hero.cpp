
#include "Hero.h"
#include <iostream>
#include <limits>

Hero::Hero(const std::string& name)
    : name_(name)
    , xp_(0)
    , level_(1)
    , hp_(25)
    , maxhp_(25)
    , baseStrength_(4)
    , statPoints_(0)
    , gold_(0)
    , equippedIndex_(-1)
{}

Hero::Hero(const std::string& name,
           int xp,
           int level,
           int hp,
           int maxhp,
           int baseStrength,
           int statPoints)
    : name_(name)
    , xp_(xp)
    , level_(level)
    , hp_(hp)
    , maxhp_(maxhp)
    , baseStrength_(baseStrength)
    , statPoints_(statPoints)
    , gold_(0)
    , equippedIndex_(-1)
{}

// — Getters
const std::string& Hero::getName() const { return name_; }
int Hero::getXp()       const { return xp_; }
int Hero::getLevel()    const { return level_; }
int Hero::getHp()       const { return hp_; }
int Hero::getMaxHp()    const { return maxhp_; }
int Hero::getStatPoints() const { return statPoints_; }
int Hero::getGold()     const { return gold_; }

// Returnerer base + bonus fra aktuelt udstyret våben
int Hero::attack() const {
    if (equippedIndex_ >= 0 && equippedIndex_ < (int)weapons_.size()) {
        return baseStrength_ + weapons_[equippedIndex_].attackBonus;
    }
    return baseStrength_;
}

void Hero::takeDamage(int amount) {
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

// Øg kill-count på det udstyrede våben
void Hero::recordKill() {
    if (equippedIndex_ >= 0 && equippedIndex_ < (int)weapons_.size()) {
        weapons_[equippedIndex_].kills++;
        std::cout << name_ << " dræbte et monster med “"
                  << weapons_[equippedIndex_].name << "” "
                  << "(total kills: " << weapons_[equippedIndex_].kills << ").\n";
    }
}

// — XP & level 
int Hero::xpThreshold() const {
    return level_ * 1000;
}
void Hero::addXp(int amount) { /* ... */ }
bool Hero::canLevelUp() const  { /* ... */ }
bool Hero::levelUp()          { /* ... */ }

// — Stat-allokering 
bool Hero::allocateStats(int hpPoints, int strengthPoints) { /* ... */ }
void Hero::restoreHp()                                { /* ... */ }
void Hero::setMaxHp(int maxhp)                        { /* ... */ }

// — Guld 
void Hero::addGold(int amount) {
    gold_ += amount;
    std::cout << name_ << " har nu " << gold_ << " guld.\n";
}

// — Inventory & equipment —
void Hero::addWeapon(const std::string& name, int attackBonus) {
    weapons_.push_back({ name, attackBonus, 0 });
    std::cout << name_ << " erhvervede våbenet “" << name
              << "” (+" << attackBonus << " styrke) til inventory.\n";
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
    std::cout << name_ << " udstyrer “"
              << weapons_[index].name << "”.\n";
    return true;
}

void Hero::showWeapons() const {
    if (weapons_.empty()) {
        std::cout << name_ << " har ingen våben.\n";
        return;
    }
    std::cout << name_ << "s våben:\n";
    for (int i = 0; i < (int)weapons_.size(); ++i) {
        const auto& w = weapons_[i];
        std::cout << " " << (i+1) << ". " << w.name
                  << " (+" << w.attackBonus << " styrke, "
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
