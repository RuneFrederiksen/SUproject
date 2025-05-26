#ifndef HERO_H
#define HERO_H

#include <string>
#include <vector>

struct Weapon {
    std::string name;
    int damage;
    double strengthModifier;
    int durability;
    int kills;
};

class Hero {
public:
    Hero(const std::string& name);
    Hero(const std::string& name,
         int xp, int level, int hp, int maxhp,
         int strength, int statPoints);

    // getters…
    const std::string& getName() const;
    int getXp() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getStrength() const;
    int getStatPoints() const;
    int getGold() const;
    int getEquippedIndex() const;

    // combat & weapons
    int attack();
    void takeDamage(int);
    void recordKill();

    // xp/level
    int xpThreshold() const;
    void addXp(int);
    bool canLevelUp() const;
    bool levelUp();

    // stats
    bool allocateStats(int hpP,int strP);
    void restoreHp();
    void setMaxHp(int);

    // gold
    void addGold(int);

    // inventory
    void addWeapon(const Weapon& w);
    bool equipWeapon(int idx);
    void showWeapons();
    const std::vector<Weapon>& getWeapons() const;

private:
    std::string name_;
    int xp_, level_, hp_, maxhp_, strength_, statPoints_, gold_;
    std::vector<Weapon> weapons_;
    int equippedIndex_;
};

#endif // HERO_H
