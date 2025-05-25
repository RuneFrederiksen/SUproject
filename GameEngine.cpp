// GameEngine.cpp
#include "GameEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

GameEngine::GameEngine(const std::string& dbFile)
  : hero_(nullptr)
  , grotte_()
  , shop_()
  , db_(dbFile)
{}

void GameEngine::run() {
    while (true) {
        showMainMenu();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

void GameEngine::showMainMenu() {
    std::cout << "\n=== Hovedmenu ===\n"
              << "1. New Game\n"
              << "2. Load Game\n"
              << "3. Save & Exit\n"
              << "4. Analyse\n"
              << "5. Exit Without Saving\n"
              << "Choice: ";
    int choice = getChoice(1, 5);

    switch (choice) {
        case 1: newGame();      break;
        case 2: loadGame();     break;
        case 3: saveAndExit();  break;
        case 4: db_.run();      break;
        case 5:
        default: std::exit(0);
    }
}

void GameEngine::newGame() {
    std::cout << "Enter hero name: ";
    std::string name;
    std::cin >> name;

    hero_ = std::make_unique<Hero>(name);
    db_.saveHero(*hero_);
    db_.saveWeaponKills(*hero_);

    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }
    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void GameEngine::loadGame() {
    auto names = db_.listHeroes();
    if (names.empty()) {
        std::cout << "Ingen helte fundet.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }
    std::cout << "Vælg en helt:\n";
    for (size_t i = 0; i < names.size(); ++i) {
        std::cout << (i+1) << ". " << names[i] << "\n";
    }
    int idx = getChoice(1, (int)names.size());

    Hero tmp("",0,0,0,0,0,0);
    if (!db_.loadHero(names[idx-1], tmp)) {
        std::cout << "Load fejlede for " << names[idx-1] << ".\n";
        return;
    }

    hero_ = std::make_unique<Hero>(tmp);
    for (auto& w : db_.loadWeaponKills(tmp.getName())) {
        hero_->addWeapon(w);
    }

    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }
    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void GameEngine::showAdventureMenu() {
    std::cout << "\n[ " << hero_->getName()
              << " — HP: "   << hero_->getHp()   << "/" << hero_->getMaxHp()
              << ", Skade: " << hero_->getStrength()
              << ", XP: "    << hero_->getXp()
              << ", Guld: "  << hero_->getGold()
              << " ]\n";

    std::cout << "\n=== Hvad vil du gøre? ===\n"
              << "1. Level Up\n"
              << "2. Heal Up\n"
              << "3. Gå ind i grotten\n"
              << "4. Åbn Butik\n"
              << "5. Vis Inventory\n"
              << "6. Tilbage til Hovedmenu\n"
              << "Choice: ";
    int choice = getChoice(1, 6);

    switch (choice) {
        case 1: manualLevelUp();               break;
        case 2:
            hero_->restoreHp();
            std::cout << hero_->getName()
                      << " er fuldt helbredt (-100 XP)\n";
            break;
        case 3: grotte_.enter(*hero_);  db_.saveHero(*hero_); db_.saveWeaponKills(*hero_);  break;
        case 4: shop_.open(*hero_);            break;
        case 5: hero_->showWeapons();          break;
        case 6: showMainMenu();
        default: return;                       // tilbage til hovedmenu
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void GameEngine::manualLevelUp() {
    if (!hero_->canLevelUp()) {
        std::cout << "Du kan ikke level up nu.\n";
        return;
    }
    hero_->levelUp();
    std::cout << "Tillykke! Nu niveau " << hero_->getLevel() << ".\n";
    allocateStats();
}

void GameEngine::allocateStats() {
    while (hero_->getStatPoints() > 0) {
        std::cout << "\nDu har " << hero_->getStatPoints()
                  << " stat points.\n"
                  << "1) HP  2) Styrke  3) Færdig\n"
                  << "Choice: ";
        int c = getChoice(1, 3);
        if (c == 3) break;
        std::cout << "Hvor mange? ";
        int pts = getChoice(0, hero_->getStatPoints());
        if (c == 1) hero_->allocateStats(pts, 0);
        else        hero_->allocateStats(0, pts);
    }
}

void GameEngine::saveAndExit() {
    if (!hero_) {
        std::cout << "Intet spil at gemme.\n";
    } else {
        db_.saveHero(*hero_);
        db_.saveWeaponKills(*hero_);
        std::cout << "Spillet er gemt. Afslutter...\n";
    }
    std::exit(0);
}

int GameEngine::getChoice(int min, int max) const {
    int c;
    while (!(std::cin >> c) || c < min || c > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n');
        std::cout << "Ugyldigt valg (" << min << "-" << max << "): ";
    }
    return c;
}
