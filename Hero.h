
#ifndef HERO_H
#define HERO_H

#include <string>
#include <vector>

struct Weapon {
    std::string name;
    int attackBonus;
    int kills;          // Antal monstre dræbt med dette våben
};

class Hero {
public:
    Hero(const std::string& name);
    Hero(const std::string& name,
         int xp,
         int level,
         int hp,
         int maxhp,
         int baseStrength,
         int statPoints);

    // Getters
    const std::string& getName() const;
    int getXp() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getStatPoints() const;
    int getGold() const;

    // Combat
    int attack() const;             
    void takeDamage(int amount);

    // Når et monster dræbes
    void recordKill();

    // XP & level
    int xpThreshold() const;
    void addXp(int amount);
    bool canLevelUp() const;
    bool levelUp();

    // Stat-allokering
    bool allocateStats(int hpPoints, int strengthPoints);
    void restoreHp();
    void setMaxHp(int maxhp);

    // Guld
    void addGold(int amount);

    // Inventory & equipment
    void addWeapon(const std::string& name, int attackBonus);
    bool equipWeapon(int index);    
    void showWeapons() const;

private:
    std::string name_;
    int xp_;
    int level_;
    int hp_;
    int maxhp_;
    int baseStrength_;
    int statPoints_;
    int gold_;

    std::vector<Weapon> weapons_;
    int equippedIndex_;    // -1 hvis intet våben er udstyret
};

#endif // HERO_H
