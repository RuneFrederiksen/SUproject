#ifndef DB_MANAGER_H
#define DB_MANAGER_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "Hero.h"

class DBManager {
public:
    explicit DBManager(const std::string& filename = "game.db");
    ~DBManager();

    // Open/initialize
    bool openConnection();
    void closeConnection();

    // Hero persistence
    void saveHero(const Hero& hero);
    bool loadHero(const std::string& name, Hero& outHero);
    std::vector<std::string> getAllHeroNames() const;

    // Weapon‐kill stats
    void saveWeaponStats(const Hero& hero);
    std::vector<Weapon> loadWeaponStats(const std::string& heroName) const;

    // Analysis menu
    void runAnalysisMenu();

private:
    sqlite3* db_;
    std::string dbFile_;

    // Schema setup
    void ensureSchema();

    // Analysis helpers
    void showAnalysisOptions() const;
    int  readChoice(int min, int max) const;
    void listHeroesAlpha() const;
    void reportTotalKills() const;
    void reportKillsByWeapon() const;
    void reportTopHeroPerWeapon() const;
};

#endif
