#ifndef GROTTE_H
#define GROTTE_H

#include <vector>
#include <random>
#include <string>
#include <numeric>
#include "Enemy.h"
#include "Hero.h"

class Grotte {
public:
    Grotte();
    void enter(Hero& hero);

private:
    struct Cave {
        std::string name;
        int goldReward;
        std::vector<Enemy> enemies;
        bool cleared = false;
    };

    std::vector<Cave> caves_;
    std::mt19937      rng_;

    void generateCaves(const Hero& hero);
    bool fight(Hero& hero, Enemy enemy);
    void runCave(Cave& cave, Hero& hero);
};

#endif 
