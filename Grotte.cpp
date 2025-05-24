
#include "Grotte.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <random>
#include <thread>
#include <chrono>

// Hjælpefunktion til at indsætte pauser
inline void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

Grotte::Grotte()
  : rng_(std::random_device{}())
{}

void Grotte::enter(Hero& hero) {
    generateCaves(hero);
    if (caves_.empty()) {
        std::cout << "Ingen grotter er tilgængelige.\n" << std::flush;
        delay(500);
        return;
    }

    while (true) {
        if (caves_.empty()) {
            std::cout << "Alle grotter er gennemført. Tilbage til hovedmenu.\n" << std::flush;
            delay(500);
            break;
        }

        std::cout << "\n[Din HP: " << hero.getHp() << "/" << hero.getMaxHp()
                  << ", Skade: " << hero.attack()
                  << ", XP: " << hero.getXp()
                  << ", Guld: " << hero.getGold() << "]\n" << std::flush;
        delay(300);

        std::cout << "=== Tilgængelige grotter ===\n" << std::flush;
        delay(200);

        for (size_t i = 0; i < caves_.size(); ++i) {
            const auto& c = caves_[i];
            std::cout << (i+1) << ". " << c.name
                      << " - Guld: " << c.goldReward
                      << ", Fjender: " << c.enemies.size() << "\n" << std::flush;
            delay(100);

            std::cout << "   Fjender:\n" << std::flush;
            delay(100);
            for (const auto& e : c.enemies) {
                std::cout << "     - " << e.getName()
                          << " (HP: " << e.getHp()
                          << ", DMG: " << e.getDamage()
                          << ", XP: " << e.getXpReward() << ")\n" << std::flush;
                delay(50);
            }
        }

        std::cout << "Vælg grotter at udfordre 1-3 eller 0 for at afslutte: " << std::flush;
        delay(100);

        std::string line;
        std::getline(std::cin >> std::ws, line);
        if (line == "0") break;

        std::vector<int> choices;
        std::stringstream ss(line);
        int idx;
        while (ss >> idx) {
            if (idx >= 1 && idx <= static_cast<int>(caves_.size())) {
                choices.push_back(idx - 1);
            }
            if (ss.peek() == ',') ss.ignore();
        }
        if (choices.empty()) {
            std::cout << "Ingen gyldige valg. Prøv igen.\n" << std::flush;
            delay(300);
            continue;
        }

        for (int ci : choices) {
            auto& cave = caves_[ci];
            runCave(cave, hero);
            if (hero.getHp() <= 0) return;
            cave.cleared = true;
        }

        caves_.erase(
            std::remove_if(caves_.begin(), caves_.end(),
                [](const Cave& c) { return c.cleared; }),
            caves_.end()
        );
    }
}

void Grotte::generateCaves(const Hero& hero) {
    caves_.clear();
    int level = hero.getLevel();
    int numCaves = 3;

    const std::vector<std::pair<std::string,int>> typeUnlocks = {
        {"Goblin", 1}, {"Skeleton", 1}, {"Orc", 2},
        {"Bandit", 3}, {"Troll", 4},    {"Dragon", 5}
    };
    const std::vector<std::tuple<int,int,int>> baseStats = {
        {3,  2, 150}, {1,  2, 200}, {5, 3, 300},
        {7, 4, 400}, {10, 5, 500}, {12, 6, 1000}
    };

    std::vector<std::string> available;
    for (size_t i = 0; i < typeUnlocks.size(); ++i) {
        if (level >= typeUnlocks[i].second)
            available.push_back(typeUnlocks[i].first);
    }
    available.push_back("Mixed");
    std::shuffle(available.begin(), available.end(), rng_);

    const int weakChance = 20;
    const int strongChance = 30;
    std::uniform_int_distribution<int> variantDist(1, 100);

    for (int i = 0; i < numCaves; ++i) {
        std::string theme = available[i % available.size()];
        std::string caveName = (theme == "Mixed")
            ? ("Mixed Cave " + std::to_string(i+1))
            : (theme + " Cave");

        int goldReward = level * 10 * (i+1);
        std::vector<Enemy> enemies;
        int numEnemies = level + (i+1);

        std::vector<int> allowed;
        if (theme == "Mixed") {
            for (size_t t = 0; t < typeUnlocks.size(); ++t)
                if (level >= typeUnlocks[t].second) allowed.push_back(t);
        } else {
            for (size_t t = 0; t < typeUnlocks.size(); ++t)
                if (typeUnlocks[t].first == theme) { allowed.push_back(t); break; }
        }

        for (int j = 0; j < numEnemies; ++j) {
            int bi = allowed[ std::uniform_int_distribution<int>(0, allowed.size()-1)(rng_) ];
            auto [hp, dmg, xp] = baseStats[bi];

            hp  += level * 2;
            dmg += level;
            xp  += level * 5;

            std::string name = typeUnlocks[bi].first;
            int roll = variantDist(rng_);
            if (roll <= weakChance) {
                name = "Weak " + name;
                hp  /= 2;
                xp  /= 2;
            }
            else if (roll <= weakChance + strongChance) {
                name = "Strong " + name;
                hp  *= 2;
                xp  *= 2;
            }

            enemies.emplace_back(name, hp, dmg, xp);
        }

        caves_.push_back({caveName, goldReward, std::move(enemies), false});
    }
}

bool Grotte::fight(Hero& hero, Enemy enemy) {
    std::cout << "\nEn kamp mod " << enemy.getName() << "!\n" << std::flush;
    delay(300);
    while (hero.getHp() > 0 && enemy.isAlive()) {
        int dmg = hero.attack();
        std::cout << "Du angriber " << enemy.getName()
                  << " for " << dmg << " skade.\n" << std::flush;
        enemy.takeDamage(dmg);
        delay(200);
        if (!enemy.isAlive()) break;

        int edmg = enemy.attack();
        std::cout << enemy.getName() << " angriber dig for "
                  << edmg << " skade.\n" << std::flush;
        hero.takeDamage(edmg);
        delay(200);
    }
    if (hero.getHp() <= 0) {
        std::cout << "Du døde i kampen!\n" << std::flush;
        return false;
    } else {
        std::cout << "Du besejrede " << enemy.getName() << "!\n" << std::flush;
        hero.addXp(enemy.getXpReward());
        return true;
    }
}

void Grotte::runCave(Cave& cave, Hero& hero) {
    std::cout << "\nGår ind i " << cave.name << "...\n" << std::flush;
    delay(500);
    for (auto& e : cave.enemies) {
        if (!fight(hero, e)) {
            std::cout << "Du kunne ikke fuldføre " << cave.name << ".\n" << std::flush;
            delay(300);
            return;
        }
    }
    std::cout << "\nDu besejrede alle fjender i " << cave.name << "!\n"
              << "Du modtog " << cave.goldReward << " guld.\n" << std::flush;
    delay(400);
    hero.addGold(cave.goldReward);
}
