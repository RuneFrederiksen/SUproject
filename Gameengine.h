#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <random>
#include <vector>
#include <string>

#include "Hero.h"
#include "Enemy.h"

class GameEngine {
public:
    GameEngine();
    void run();

private:
    void showMainMenu();
    void newGame();
    void loadGame();
    void saveGame() const;
    void setupEnemies();
    void enterRoom();
    void battle(Enemy& enemy);
    void manualLevelUp();
    void allocateStats();
    int getChoice(int min, int max) const;

    std::unique_ptr<Hero> hero_;
    std::vector<Enemy> enemies_;
    std::mt19937 rng_;
};

#endif
