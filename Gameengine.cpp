// GameEngine.cpp

#include "GameEngine.h"
#include "Shop.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <limits>

GameEngine::GameEngine()
{
}

void GameEngine::run()
{
    while (true) {
        showMainMenu();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GameEngine::showMainMenu()
{
    std::cout << "=== Hovedmenu ===\n"
              << "1. New Game\n"
              << "2. Load Game\n"
              << "3. Exit\n"
              << "Choice: ";
    int choice = getChoice(1, 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    switch (choice) {
        case 1: newGame(); break;
        case 2: loadGame(); break;
        case 3:
        default: std::exit(0);
    }
}

void GameEngine::newGame()
{
    std::cout << "Enter hero name: ";
    std::string name;
    std::cin >> name;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    hero_ = std::make_unique<Hero>(name);

    // Main adventure loop
    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }

    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void GameEngine::loadGame()
{
    std::ifstream in("savegame.dat", std::ios::binary);
    if (!in) {
        std::cout << "Ingen gemning fundet.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }

    std::string name;
    int xp, level, hp, maxhp, strength, statPoints, gold;
    std::getline(in, name);
    in >> xp >> level >> hp >> maxhp >> strength >> statPoints >> gold;
    in.close();

    hero_ = std::make_unique<Hero>(name, xp, level, hp, maxhp, strength, statPoints);
    hero_->addGold(gold - hero_->getGold());

    std::cout << "Game loaded for " << name << ".\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }

    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void GameEngine::saveGame() const
{
    if (!hero_) return;

    std::ofstream out("savegame.dat", std::ios::binary);
    out << hero_->getName() << "\n"
        << hero_->getXp()       << " "
        << hero_->getLevel()    << " "
        << hero_->getHp()       << " "
        << hero_->getMaxHp()    << " "
        << hero_->getStrength() << " "
        << hero_->getStatPoints()<< " "
        << hero_->getGold()     << "\n";
    out.close();

    std::cout << "Game saved.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void GameEngine::showAdventureMenu()
{
    if (hero_) {
        std::cout << "\n[ " << hero_->getName()
                  << " — HP: "   << hero_->getHp()   << "/" << hero_->getMaxHp()
                  << ", Skade: " << hero_->getStrength()
                  << ", XP: "    << hero_->getXp()
                  << ", Guld: "  << hero_->getGold()
                  << " ]\n";
    }

    std::cout << "\n=== Hvad vil du gøre? ===\n"
              << "1. Level Up\n"
              << "2. Heal Up\n"
              << "3. Gå ind i grotten\n"
              << "4. Åbn Butik\n"
              << "5. Vis Inventory\n"
              << "6. Exit and Save Game\n"
              << "Choice: ";
    int choice = getChoice(1, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    switch (choice) {
        case 1:
            manualLevelUp();
            break;
        case 2:
            if (hero_) {
                hero_->restoreHp();
                std::cout << hero_->getName() << " er fuldt helbredt -100xp.\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
            break;
        case 3:
            if (hero_) {
                grotte_.enter(*hero_);
            }
            break;
        case 4:
            if (hero_) {
                shop_.open(*hero_);
            }
            break;
        case 5:
            if (hero_) {
                hero_->showWeapons();
            }
            break;
        case 6:
        default:
            saveGame();
            std::exit(0);
    }
}

void GameEngine::manualLevelUp()
{
    if (!hero_) return;

    if (!hero_->canLevelUp()) {
        std::cout << "Du kan ikke level up nu.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }

    hero_->levelUp();
    std::cout << "Tillykke! Du er nu niveau " << hero_->getLevel() << ".\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    allocateStats();
}

void GameEngine::allocateStats()
{
    if (!hero_) return;

    while (hero_->getStatPoints() > 0) {
        std::cout << "\nDu har " << hero_->getStatPoints() << " stat points.\n"
                  << "1. Tilføj til HP\n"
                  << "2. Tilføj til Styrke\n"
                  << "3. Færdig\n"
                  << "Choice: ";
        int choice = getChoice(1, 3);
        if (choice == 3) break;

        std::cout << "Hvor mange points vil du tilføje? ";
        int pts = getChoice(0, hero_->getStatPoints());

        if (choice == 1) {
            hero_->allocateStats(pts, 0);
            std::cout << "Max HP er nu " << hero_->getMaxHp() << ".\n";
        } else if (choice == 2) {
            hero_->allocateStats(0, pts);
            std::cout << "Styrke er nu " << hero_->getStrength() << ".\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int GameEngine::getChoice(int min, int max) const
{
    int choice;
    while (true) {
        if (!(std::cin >> choice) || choice < min || choice > max) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ugyldigt valg (" << min << "-" << max << "): ";
        } else {
            break;
        }
    }
    return choice;
}
