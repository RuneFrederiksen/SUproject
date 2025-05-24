// Hero.h
#ifndef HERO_H
#define HERO_H

#include <string>
#include <vector>

struct Weapon {
    std::string name;
    int attackBonus;
    int kills;   // number of monsters killed with this weapon
};

class Hero {
public:
    Hero(const std::string& name);
    Hero(const std::string& name,
         int xp,
         int level,
         int hp,
         int maxhp,
         int strength,
         int statPoints);

    // Getters
    const std::string& getName() const;
    int getXp() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getStrength() const;    // base strength without weapon bonus
    int getStatPoints() const;
    int getGold() const;

    // Combat
    int attack() const;         // strength + bonus from equipped weapon
    void takeDamage(int amount);
    void recordKill();          // increment kill count on equipped weapon

    // XP & leveling
    int xpThreshold() const;
    void addXp(int amount);
    bool canLevelUp() const;
    bool levelUp();

    // Stat allocation
    bool allocateStats(int hpPoints, int strengthPoints);
    void restoreHp();           // restores to max HP, costs 100 XP
    void setMaxHp(int maxhp);

    // Gold
    void addGold(int amount);

    // Inventory & equipment
    void addWeapon(const std::string& name, int attackBonus);
    bool equipWeapon(int index);  
    void showWeapons();          // lists weapons and allows equipping

private:
    std::string name_;
    int xp_;
    int level_;
    int hp_;
    int maxhp_;
    int strength_;     // base strength
    int statPoints_;
    int gold_;

    std::vector<Weapon> weapons_;
    int equippedIndex_;  // index into weapons_, -1 if none equipped
};

#endif // HERO_H
