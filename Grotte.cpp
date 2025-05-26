#include "Grotte.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <random>
#include <thread>
#include <chrono>

// Hjælpefunktion til at indsætte korte pauser, så spillet føles mere levende
inline void delay(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Konstruktør: initialiserer random-generatoren
Grotte::Grotte()
  : rng_(std::random_device{}())
{}

// Metode til at gå ind i grotterne og vælge udfordringer
void Grotte::enter(Hero& hero) {
    // Generér et sæt huler baseret på helteniveau
    generateCaves(hero);

    // Hvis der ingen grotter er, afbryd straks
    if (caves_.empty()) {
        std::cout << "Ingen grotter er tilgængelige." << std::endl;
        delay(500);
        return;
    }

    // Hovedloop: vis grottevalg indtil alle er klaret eller bruger annullerer
    while (true) {
        if (caves_.empty()) {
            std::cout << "Alle grotter er gennemført. Tilbage til hovedmenu." << std::endl;
            delay(500);
            break;
        }

        // Vis heltestatus over grottevalg
        std::cout << "\n[Din HP: " << hero.getHp() << "/" << hero.getMaxHp()
                  << ", Skade: " << hero.getStrength()
                  << ", XP: " << hero.getXp()
                  << ", Guld: " << hero.getGold() << "]" << std::endl;
        delay(300);

        // Overskrift for grottevalg
        std::cout << "=== Tilgængelige grotter ===" << std::endl;
        delay(200);

        // List alle grotter med detaljer om fjender
        for (size_t i = 0; i < caves_.size(); ++i) {
            const auto& c = caves_[i];
            std::cout << (i+1) << ". " << c.name
                      << " - Guld: " << c.goldReward
                      << ", Fjender: " << c.enemies.size() << std::endl;
            delay(100);

            // Vis fjendelisten for hver grotte
            std::cout << "   Fjender:" << std::endl;
            delay(100);
            for (const auto& e : c.enemies) {
                std::cout << "     - " << e.getName()
                          << " (HP: " << e.getHp()
                          << ", DMG: " << e.getDamage()
                          << ", XP: " << e.getXpReward() << ")" << std::endl;
                delay(50);
            }
        }

        // Prompt brugeren om valg af grotte(r)
        std::cout << "Vælg grotter at udfordre 1-3 eller 0 for at afslutte:" << std::endl;
        std::cout << "> ";
        std::flush(std::cout);
        delay(100);

        // Læs indtastning (håndterer resterende newline korrekt)
        std::string line;
        std::getline(std::cin, line);
        if (line.empty()) {
            std::getline(std::cin, line);
        }
        if (line == "0") break;

        // Parser flere valg adskilt af mellemrum eller komma
        std::vector<int> choices;
        std::stringstream ss(line);
        int idx;
        while (ss >> idx) {
            if (idx >= 1 && idx <= static_cast<int>(caves_.size())) {
                choices.push_back(idx - 1);
            }
            if (ss.peek() == ',') ss.ignore();
        }

        // Hvis ingen gyldigt valg, vis fejl og restart loop
        if (choices.empty()) {
            std::cout << "Ingen gyldige valg. Prøv igen." << std::endl;
            delay(300);
            continue;
        }

        // Kør hver valgt grotte i rækkefølge
        for (int ci : choices) {
            auto& cave = caves_[ci];
            runCave(cave, hero);
            if (hero.getHp() <= 0) return;  // stop hvis helten dør
            cave.cleared = true;
            delay(300);  // kort pause mellem grotter
        }

        // Fjern gennemførte grotter fra listen
        caves_.erase(
            std::remove_if(caves_.begin(), caves_.end(),
                [](const Cave& c) { return c.cleared; }),
            caves_.end()
        );
    }
}

// Genererer hulernes navn og fjendesammensætning ud fra helteniveau
void Grotte::generateCaves(const Hero& hero) {
    caves_.clear();
    int level = hero.getLevel();
    int numCaves = 3;

    // Typelås: hvilke fjendetyper er tilgængelige ved hvert niveau
    const std::vector<std::pair<std::string,int>> typeUnlocks = {
        {"Goblin", 1}, {"Skeleton", 1}, {"Orc", 2},
        {"Bandit", 3}, {"Troll", 4},    {"Dragon", 5}
    };
    // Grundstatistik for hver type
    const std::vector<std::tuple<int,int,int>> baseStats = {
        {3,  2, 150}, {1,  2, 200}, {5, 3, 300},
        {6, 4, 400}, {8, 5, 500}, {10, 6, 1000}
    };

    // Sammensæt liste over tilladte temaer og bland dem
    std::vector<std::string> available;
    for (size_t i = 0; i < typeUnlocks.size(); ++i) {
        if (level >= typeUnlocks[i].second)
            available.push_back(typeUnlocks[i].first);
    }
    available.push_back("Mixed");
    std::shuffle(available.begin(), available.end(), rng_);

    // Sandsynlighedsvægte for varianter (Weak, Strong)
    const int weakChance = 20;
    const int strongChance = 20;
    std::uniform_int_distribution<int> variantDist(1, 100);

    // Opret hver grotte med passende antal fjender
    for (int i = 0; i < numCaves; ++i) {
        std::string theme = available[i % available.size()];
        std::string caveName = (theme == "Mixed")
            ? ("Mixed Cave " + std::to_string(i+1))
            : (theme + " Cave");

        int goldReward = level * 10 * (i+1);
        std::vector<Enemy> enemies;
        int numEnemies = 1 + (i+1);
        

        // Vælg hvilke typer der må indgå
        std::vector<int> allowed;
        if (theme == "Mixed") {
            for (size_t t = 0; t < typeUnlocks.size(); ++t)
                if (level >= typeUnlocks[t].second) allowed.push_back(t);
        } else {
            for (size_t t = 0; t < typeUnlocks.size(); ++t)
                if (typeUnlocks[t].first == theme) { allowed.push_back(t); break; }
        }

        // Skab fjender med tilfældige varianter
        for (int j = 0; j < numEnemies; ++j) {
            int bi = allowed[ std::uniform_int_distribution<int>(0, 
                       static_cast<int>(allowed.size())-1)(rng_) ];
            auto [hp, dmg, xp] = baseStats[bi];

            hp  += level * 1.5;
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

// Fører en enkelt kamp mellem helten og en fjende
bool Grotte::fight(Hero& hero, Enemy enemy) {
    std::cout << "\nEn kamp mod " << enemy.getName() << "!" << std::endl;
    delay(300);
    while (hero.getHp() > 0 && enemy.isAlive()) {
        // Helten angriber først
        int dmg = hero.attack();
        std::cout << "Du angriber " << enemy.getName()
                  << " for " << dmg << " skade." << std::endl;
        delay(150);
        enemy.takeDamage(dmg);
        delay(150);

        // Hvis fjenden dør, registrér kill og afslut
        if (!enemy.isAlive()) {
            hero.recordKill();
            delay(200);
            break;
        }

        // Ellers angriber fjenden tilbage
        int edmg = enemy.attack();
        std::cout << enemy.getName() << " angriber dig for "
                  << edmg << " skade." << std::endl;
        delay(150);
        hero.takeDamage(edmg);
        delay(150);
    }

    // Returner false hvis helten døde, ellers true efter XP-givning
    if (hero.getHp() <= 0) {
        std::cout << "Du døde i kampen!" << std::endl;
        delay(500);
        return false;
    } else {
        std::cout << "Du besejrede " << enemy.getName() << "!" << std::endl;
        delay(300);
        hero.addXp(enemy.getXpReward());
        delay(200);
        return true;
    }
}

// Kører alle kampe i valgt grotte og giver belønning ved succes
void Grotte::runCave(Cave& cave, Hero& hero) {
    std::cout << "\nGår ind i " << cave.name << "..." << std::endl;
    delay(500);
    for (auto& e : cave.enemies) {
        if (!fight(hero, e)) {
            std::cout << "Du kunne ikke fuldføre " << cave.name << "." << std::endl;
            delay(300);
            return;
        }
    }
    std::cout << "\nDu besejrede alle fjender i " << cave.name << "!\n"
              << "Du modtog " << cave.goldReward << " guld." << std::endl;
    delay(400);
    hero.addGold(cave.goldReward);
    delay(200);
}
