// GameEngine.h
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <string>
#include "Hero.h"
#include "Shop.h"
#include "Grotte.h"
#include "Database.h"

class GameEngine {
public:
    explicit GameEngine(const std::string& dbFile = "game.db");
    void run();

private:
    void showMainMenu();
    void newGame();
    void loadGame();
    void showAdventureMenu();
    void manualLevelUp();
    void allocateStats();
    void saveAndExit();
    int getChoice(int min, int max) const;

    std::unique_ptr<Hero> hero_;
    Grotte              grotte_;
    Shop                shop_;
    Database            db_;
};

#endif // GAMEENGINE_H
