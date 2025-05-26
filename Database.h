#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "Hero.h"

// Database wraps all SQLite logic: schema setup, CRUD for heroes & weapon kills, and analysis menu.
class Database {
public:
    explicit Database(const std::string& filename = "game.db");
    ~Database();

    bool openConnection();
    void closeConnection();

    void saveHero(const Hero& hero);
    bool loadHero(const std::string& name, Hero& outHero);
    std::vector<std::string> getAllHeroNames() const;

    void saveWeaponStats(const Hero& hero);
    std::vector<Weapon> loadWeaponStats(const std::string& heroName) const;

    void runAnalysisMenu();

private:
    sqlite3*    db_;      // SQLite connection handle
    std::string dbFile_;  // Path to .db file

    void ensureSchema();

    void showAnalysisOptions() const;
    int  readChoice(int min, int max) const;
    void listHeroesAlpha() const;
    void reportTotalKills() const;
    void reportKillsByWeapon() const;
    void reportTopHeroPerWeapon() const;
};

#endif
