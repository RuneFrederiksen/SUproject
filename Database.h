#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "Hero.h"
#include <sqlite3.h>

class Database {
public:
    explicit Database(const std::string& dbFile);
    ~Database();

    // Starter analyse‐menuen
    void run();

    // Schema + CRUD
    void initSchema();
    void saveHero(const Hero& hero);
    bool loadHero(const std::string& name, Hero& outHero);
    std::vector<std::string> listHeroes() const;

    void saveWeaponKills(const Hero& hero);
    std::vector<Weapon> loadWeaponKills(const std::string& heroName) const;

private:
    sqlite3* db_;

    void executeNonQuery(const std::string& sql);
    void executeSimpleQuery(const std::string& sql) const;

    // Hjælpemetoder til run()
    void showMenu() const;
    int getChoice(int min, int max) const;
    void listHeroesAlphabetical() const;
    void showTotalKillsPerHero() const;
    void showKillsByHeroPerWeapon() const;
    void showTopHeroPerWeapon() const;
};

#endif // DATABASE_H
