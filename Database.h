// Database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "Hero.h"

class Database {
public:
    explicit Database(const std::string& dbFile = "game.db");
    ~Database();

    // Analysis menu
    void run();

    // Hero CRUD
    bool loadHero(const std::string& name, Hero& outHero);
    void saveHero(const Hero& hero);
    std::vector<std::string> listHeroes() const;

    // Weapon-kills CRUD
    std::vector<Weapon> loadWeaponKills(const std::string& heroName) const;
    void saveWeaponKills(const Hero& hero);

private:
    sqlite3* db_;
    bool     isOpen_;

    // Initialization
    bool openDatabase(const std::string& path);
    void closeDatabase();
    void initSchema();

    // Analysis helpers
    void showMenu() const;
    int  getChoice(int min, int max) const;
    void listHeroesAlphabetical() const;
    void showTotalKillsPerHero() const;
    void showKillsByHeroPerWeapon() const;
    void showTopHeroPerWeapon() const;
};

#endif // DATABASE_H
