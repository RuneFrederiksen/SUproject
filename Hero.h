// Hero.h
#ifndef HERO_H
#define HERO_H

#include <string>

class Hero {
public:
    // Constructor for a new Hero with default stats
    explicit Hero(const std::string& name);

    // Constructor for loading a saved Hero
    Hero(const std::string& name, int xp, int level, int hp, int strength, int statPoints = 0);

    // Getters
    const std::string& getName() const;
    int getXp() const;
    int getLevel() const;
    int getHp() const;
    int getStrength() const;
    int getStatPoints() const;

    // Combat methods
    int attack() const;
    void takeDamage(int amount);

    // Experience and leveling
    void addXp(int amount);
    bool canLevelUp() const;
    bool levelUp();            // Triggered af bruger

    // Fordel stat‐points efter level‐up
    bool allocateStats(int hpPoints, int strengthPoints);

private:
    std::string name_;
    int xp_;
    int level_;
    int hp_;
    int strength_;
    int statPoints_;           // Ubrugte points

    int xpThreshold() const;
};

#endif // HERO_H
