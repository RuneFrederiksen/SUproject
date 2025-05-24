#pragma once

#include <memory>
#include "Hero.h"
#include "Enemy.h"
#include "grotte.h"
#include "Shop.h" 

class GameEngine {
public:
    GameEngine();
    void run();

private:
    void showMainMenu();
    void newGame();
    void loadGame();
    void saveGame() const;
    void showAdventureMenu();

    void manualLevelUp();
    void allocateStats();
    int  getChoice(int min, int max) const;

    std::unique_ptr<Hero> hero_;
    Grotte                 grotte_;   
    Shop shop_; 
};
