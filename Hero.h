#ifndef HERO_H
#define HERO_H

#include <string>

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

    const std::string& getName() const;
    int getXp() const;
    int getLevel() const;
    int getHp() const;
    int getMaxHp() const;
    int getStrength() const;
    int getStatPoints() const;

    int getGold() const;           
    void addGold(int amount);     

    int attack() const;
    void takeDamage(int amount);
    int xpThreshold() const;
    void addXp(int amount);
    bool canLevelUp() const;
    bool levelUp();
    bool allocateStats(int hpPoints, int strengthPoints);
    void restoreHp();
    void setMaxHp(int maxhp);

private:
    std::string name_;
    int xp_;
    int level_;
    int hp_;
    int maxhp_;
    int strength_;
    int statPoints_;
    int gold_;                   
};

#endif
