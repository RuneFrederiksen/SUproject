#include "Gameengine.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <set>
#include <thread>
#include <chrono>
#include <limits>

GameEngine::GameEngine()
    : rng_(std::random_device{}())
{
    setupEnemies();
}

void GameEngine::run() {
    while (true) {
        showMainMenu();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GameEngine::showMainMenu() {
    std::cout << "1. New Game\n"
              << "2. Load Game\n"
              << "3. Exit\n"
              << "Choice: ";
    int choice = getChoice(1, 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    if (choice == 1) newGame();
    else if (choice == 2) loadGame();
    else std::exit(0);
}

void GameEngine::newGame() {
    std::cout << "Enter hero name: ";
    std::string name;
    std::cin >> name;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    hero_ = std::make_unique<Hero>(name);

    while (hero_->getHp() > 0) {
        enterRoom();
    }

    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void GameEngine::saveGame() const {
    std::cout << "Vælg save-slot (1–3): " << std::flush;
    int slot = getChoice(1, 3);

    std::string filename = "save" + std::to_string(slot) + ".txt";
    {
        std::ofstream out(filename);
        const auto& h = *hero_;
        out << h.getName()      << ' '
            << h.getXp()        << ' '
            << h.getLevel()     << ' '
            << h.getHp()        << ' '
            << h.getMaxHp()     << ' '
            << h.getStrength()  << ' '
            << h.getStatPoints()<< '\n';

        out.flush();  
    } 

    std::cout << "Game saved to slot " << slot 
              << " (" << filename << ")\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::exit(0);
}

void GameEngine::loadGame() {
    std::cout << "Vælg load-slot (1–3): " << std::flush;
    int slot = getChoice(1, 3);

    std::string filename = "save" + std::to_string(slot) + ".txt";
    std::ifstream in(filename);
    if (!in) {
        std::cout << "Ingen save-fil i slot " << slot 
                  << " (" << filename << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }

    std::string name;
    int xp, lvl, hp, maxhp, str, pts;
    in >> name >> xp >> lvl >> hp >> maxhp >> str >> pts;

    hero_ = std::make_unique<Hero>(name, xp, lvl, hp, maxhp, str, pts);
    std::cout << "Game loaded from slot " << slot << ": "
              << name << " Level " << lvl
              << " XP=" << xp
              << " HP=" << hp << "/" << maxhp << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    while (hero_->getHp() > 0) {
        enterRoom();
    }
    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void GameEngine::setupEnemies() {
    enemies_ = {
        Enemy("Hest",               4,  1,  150),
        Enemy("Weak Goblin",        4,  2,  300),
        Enemy("Strong Goblin",      8,  3,  600),
        Enemy("Den stærkeste Goblin", 15, 5, 1100),
        Enemy("Drage",             100, 10, 3000),
        Enemy("Preben",             1, 1000, 2000)
    };
}

void GameEngine::enterRoom() {
    std::uniform_int_distribution<> dist(0, enemies_.size() - 1);
    std::vector<Enemy> room;
    std::set<int> chosen;
    while (room.size() < 3) {
        int idx = dist(rng_);
        if (chosen.insert(idx).second) {
            room.push_back(enemies_[idx]);
        }
    }

    std::cout << "\n=== Dørvalg ===\n";
    std::cout << "Stats: Level=" << hero_->getLevel()
              << ", XP=" << hero_->getXp()
              << ", HP=" << hero_->getHp()
              << ", STR=" << hero_->getStrength() << "\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    for (int i = 0; i < 3; ++i) {
        const auto& e = room[i];
        std::cout << i+1 << ". " << e.getName()
                  << " (HP=" << e.getHp()
                  << ", Dmg=" << e.getDamage() << ")\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    std::cout << "4. Restore HP (Cost: 100 XP)\n"
              << "5. Gå videre\n"
              << "6. Save & Exit\n"
              << "7. Level Up (Cost: 1000 XP)\n"
              << "Choice: ";
    int c = getChoice(1, 7);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    if (c >= 1 && c <= 3) {
        battle(room[c-1]);
        if (hero_->getHp() > 0) {
            hero_->addXp(room[c-1].getXpReward());
        }
    }
    else if (c == 4) {
        if (hero_->getXp() >= 100) {
            std::cout << "Are you sure you want to restore HP for 100 XP? (y/n): ";
            char confirm; std::cin >> confirm;
            if (confirm=='y' || confirm=='Y') {
                hero_->restoreHp();
                hero_->addXp(-100);
                std::cout << "HP restored! -100 XP.\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        } else {
            std::cout << "You do not have enough XP to restore HP.\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
    else if (c == 5) {
    }
    else if (c == 6) {
        saveGame();
        std::exit(0);
    }
    else if (c == 7) {
        manualLevelUp();
        std::cout << "Opdaterede stats: Level=" << hero_->getLevel()
                  << ", XP=" << hero_->getXp()
                  << ", HP=" << hero_->getHp()
                  << ", STR=" << hero_->getStrength() << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void GameEngine::battle(Enemy& enemy) {
    std::uniform_int_distribution<int> dice(1, 6);
    int heroRoll  = dice(rng_);
    int enemyRoll = dice(rng_);
    bool heroGoesFirst = (heroRoll >= enemyRoll);

    std::cout << hero_->getName() << " kaster terning: " << heroRoll << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << enemy.getName()   << " kaster terning: " << enemyRoll << "\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << (heroGoesFirst ? hero_->getName() : enemy.getName())
              << " starter!\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    int localHp = enemy.getHp();
    int round = 1;
    while (hero_->getHp() > 0 && localHp > 0) {
        std::cout << "\n--- Runde " << round << " ---\n";
        if (heroGoesFirst) {
            int dmg = hero_->attack();
            localHp -= dmg;
            std::cout << hero_->getName()
                      << " angriber først og forårsager " << dmg
                      << " skade. " << enemy.getName()
                      << " har nu " << localHp << " HP.\n";
            if (localHp > 0) {
                dmg = enemy.attack();
                hero_->takeDamage(dmg);
                std::cout << enemy.getName()
                          << " angriber og forårsager " << dmg
                          << " skade. " << hero_->getName()
                          << " har nu " << hero_->getHp() << " HP.\n";
            }
        } else {
            int dmg = enemy.attack();
            hero_->takeDamage(dmg);
            std::cout << enemy.getName()
                      << " angriber først og forårsager " << dmg
                      << " skade. " << hero_->getName()
                      << " har nu " << hero_->getHp() << " HP.\n";
            if (hero_->getHp() > 0) {
                dmg = hero_->attack();
                localHp -= dmg;
                std::cout << hero_->getName()
                          << " angriber og forårsager " << dmg
                          << " skade. " << enemy.getName()
                          << " har nu " << localHp << " HP.\n";
            }
        }
        round++;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (hero_->getHp() > 0)
        std::cout << "\nDu besejrede " << enemy.getName() << "!\n";
    else
        std::cout << "\nDu døde i kampen mod " << enemy.getName() << "...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


void GameEngine::manualLevelUp() {
    if (!hero_ || !hero_->canLevelUp()) {
        std::cout << "Du kan ikke level up nu.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }
    hero_->levelUp();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    allocateStats();
}

void GameEngine::allocateStats() {
    int pts = hero_->getStatPoints();
    int hpPts = 0;
    int strPts = 0;
    std::cout << "Du har " << pts << " point til fordeling:\n";
    do {
        std::cout << "HP +? (0 - " << pts << "): ";
        hpPts = getChoice(0, pts);
        std::cout << "STR +? (" << (pts - hpPts) << "): ";
        strPts = getChoice(0, pts - hpPts);
        if (hpPts + strPts != pts) {
            std::cout << "Du skal bruge præcis " << pts << " point, ikke "
                      << (hpPts + strPts) << ". Prøv igen.\n";
        }
    } while (hpPts + strPts != pts);
    
    hero_->allocateStats(hpPts, strPts);
    hero_->restoreHp();
    std::cout << "Fordelt: +" << hpPts << " maxHP, +" << strPts << " STR.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

int GameEngine::getChoice(int min, int max) const {
    int choice;
    while (true) {
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ugyldigt input. Indtast et tal mellem "
                      << min << " og " << max << ": ";
            continue;
        }
        if (choice < min || choice > max) {
            std::cout << "Ugyldigt valg. Prøv igen " << '\n' << " ("
                      << min << " - " << max << "): ";
            continue;
        }
        break;
    }
    return choice;
}
