// Hero.h
#ifndef HERO_H
#define HERO_H

#include <string>
#include <vector>

struct Weapon {
    std::string name;
    int damage;               // grundskade
    double strengthModifier;  // fx 0.5 betyder +50% af hero.strength
    int durability;           // tilbageværende holdbarhed
    int kills;                // antal monstre dræbt med dette våben
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
    int getStrength() const;
    int getStatPoints() const;
    int getGold() const;

    // Combat
    int attack();             // beregn skade, opdater durability
    void takeDamage(int amount);
    void recordKill();        // +1 kills på udstyret våben

    // XP & leveling
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
    void addWeapon(const std::string& name,
                   int damage,
                   double strengthModifier,
                   int durability);
    bool equipWeapon(int index);
    void showWeapons();       // viser våben og giver mulighed for at udstyre

private:
    std::string name_;
    int xp_;
    int level_;
    int hp_;
    int maxhp_;
    int strength_;     // base styrke
    int statPoints_;
    int gold_;

    std::vector<Weapon> weapons_;
    int equippedIndex_;  // index i weapons_, -1 hvis intet udstyret
};

#endif // HERO_H
