// Enemy.h
#ifndef ENEMY_H
#define ENEMY_H

#include <string>

class Enemy {
public:
    // Constructor
    explicit Enemy(const std::string& name, int hp, int damage, int xpReward);

    // Getters
    const std::string& getName() const;
    int getHp() const;
    int getDamage() const;
    int getXpReward() const;

    // Combat methods
    int attack() const;
    void takeDamage(int amount);
    bool isAlive() const;

private:
    std::string name_;    // Enemy name
    int hp_;              // Hit points
    int damage_;          // Damage dealt per attack
    int xpReward_;        // XP given to hero when defeated
};

#endif // ENEMY_H

