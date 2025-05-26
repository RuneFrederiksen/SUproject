#include "Hero.h"
#include <iostream>
#include <limits>
#include <climits>

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
{
    // Default weapon: Bare Hands
    Weapon bareHands { "Bare Hands", 0, 0.0, 1000, 0 };
    weapons_.push_back(bareHands);
    equippedIndex_ = 0;
}

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
{
    // Default weapon: Bare Hands
    Weapon bareHands { "Bare Hands", 0, 0.0, 1000, 0 };
    weapons_.push_back(bareHands);
    equippedIndex_ = 0;
}

// —— Getters —— 

const std::string& Hero::getName() const
{
    return name_;
}

int Hero::getXp() const
{
    return xp_;
}

int Hero::getLevel() const
{
    return level_;
}

int Hero::getHp() const
{
    return hp_;
}

int Hero::getMaxHp() const
{
    return maxhp_;
}

int Hero::getStrength() const
{
    return strength_;
}

int Hero::getStatPoints() const
{
    return statPoints_;
}

int Hero::getGold() const
{
    return gold_;
}

int Hero::getEquippedIndex() const
{
    return equippedIndex_;
}

// —— Combat —— 

int Hero::attack()
{
    int totalDamage = strength_;

    if (equippedIndex_ >= 0 &&
        equippedIndex_ < static_cast<int>(weapons_.size()))
    {
        Weapon &w = weapons_[equippedIndex_];
        int bonus = w.damage
                  + static_cast<int>(strength_ * w.strengthModifier);
        totalDamage += bonus;

        std::cout << name_ << " angriber med " << w.name
                  << " (+" << bonus << " skade). ";

        w.durability--;
        std::cout << "Holdbarhed tilbage: " << w.durability << "\n";

        if (w.durability <= 0 && w.name != "Bare Hands") {
            std::cout << w.name << " er gået i stykker!\n";
            weapons_.erase(weapons_.begin() + equippedIndex_);
            equippedIndex_ = 0;  // tilbage til Bare Hands
        }
    }
    else {
        std::cout << name_ << " angriber uden våben ("
                  << strength_ << " skade).\n";
    }

    return totalDamage;
}

void Hero::takeDamage(int amount)
{
    hp_ -= amount;
    if (hp_ < 0) hp_ = 0;
}

void Hero::recordKill()
{
    if (equippedIndex_ >= 0 &&
        equippedIndex_ < static_cast<int>(weapons_.size()))
    {
        weapons_[equippedIndex_].kills++;
    }
}

// —— XP & Leveling —— 

int Hero::xpThreshold() const
{
    return level_ * 1000;
}

void Hero::addXp(int amt)
{
    xp_ = std::max(0, xp_ + amt);
    std::cout << name_ << " har nu " << xp_ << " XP.\n";

    if (canLevelUp()) {
        std::cout << name_ << " kan level up!\n";
    }
}

bool Hero::canLevelUp() const
{
    return xp_ >= xpThreshold();
}

bool Hero::levelUp()
{
    if (!canLevelUp()) {
        return false;
    }
    xp_        -= xpThreshold();
    level_     += 1;
    statPoints_ += 5;
    return true;
}

// —— Stat Allocation —— 

bool Hero::allocateStats(int hpPts, int strPts)
{
    if (hpPts < 0 || strPts < 0) return false;
    if (hpPts + strPts > statPoints_) return false;

    maxhp_       += hpPts;
    strength_    += strPts;
    statPoints_ -= (hpPts + strPts);

    if (hp_ > maxhp_) {
        hp_ = maxhp_;
    }
    return true;
}

void Hero::restoreHp()
{
    hp_ = maxhp_;
    xp_ -= 100;
}

void Hero::setMaxHp(int m)
{
    maxhp_ = m;
    if (hp_ > maxhp_) {
        hp_ = maxhp_;
    }
}

// —— Gold —— 

void Hero::addGold(int amt)
{
    gold_ += amt;
    std::cout << name_ << " har nu " << gold_ << " guld.\n";
}

// —— Inventory & Equipment —— 

void Hero::addWeapon(const Weapon& w)
{
    // Hvis allerede ejet, øg kun holdbarhed
    for (auto &owned : weapons_) {
        if (owned.name == w.name) {
            owned.durability += w.durability;
            std::cout << name_
                      << " øgede holdbarhed på “" << owned.name
                      << "” med " << w.durability
                      << ". Ny holdbarhed: " << owned.durability << "\n";
            return;
        }
    }

    // Ellers tilføj nyt våben
    weapons_.push_back(w);
    std::cout << name_
              << " tilføjede våbenet “" << w.name << "”\n";
}

bool Hero::equipWeapon(int i)
{
    if (i < 0 ||
        i >= static_cast<int>(weapons_.size()))
    {
        std::cout << "Ugyldigt valg.\n";
        return false;
    }
    equippedIndex_ = i;
    std::cout << name_
              << " equipped “" << weapons_[i].name << "”\n";
    return true;
}

void Hero::showWeapons()
{
    // Hvis ingen våben, bare sig tilbage
    if (weapons_.empty()) {
        std::cout << name_ << " har ingen våben.\n";
        return;
    }

    // List alle våben
    std::cout << name_ << "s våben:\n";
    for (int i = 0; i < static_cast<int>(weapons_.size()); ++i) {
        const Weapon &w = weapons_[i];
        std::cout << " " << (i + 1) << ". "
                  << w.name
                  << " (+" << w.damage
                  << " skade, x" << w.strengthModifier
                  << " mod., " << w.durability
                  << " holdbarhed, " << w.kills
                  << " kills)"
                  << (i == equippedIndex_ ? " [equipped]" : "")
                  << "\n";
    }

    // Giv brugeren mulighed for at udstyre eller gå tilbage
    std::cout << "Vælg våben at equippe (1-" << weapons_.size()
              << "), eller 0 for tilbage: ";
    int choice;
    while (!(std::cin >> choice)
           || choice < 0
           || choice > static_cast<int>(weapons_.size()))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ugyldigt valg (0-" << weapons_.size() << "): ";
    }

    if (choice > 0) {
        equipWeapon(choice - 1);
    }
}


const std::vector<Weapon>& Hero::getWeapons() const
{
    return weapons_;
}
