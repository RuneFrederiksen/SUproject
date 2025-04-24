#ifndef ENEMY_H
#define ENEMY_H

#include <string>

class Enemy {
public:

    explicit Enemy(const std::string& name, int hp, int damage, int xpReward);


    const std::string& getName() const;
    int getHp() const;
    int getDamage() const;
    int getXpReward() const;


    int attack() const;
    void takeDamage(int amount);
    bool isAlive() const;

private:
    std::string name_;   
    int hp_;          
    int damage_;      
    int xpReward_;        
};

#endif 

