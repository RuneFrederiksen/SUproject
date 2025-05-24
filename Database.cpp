#include "Database.h"
#include <iostream>
#include <limits>
#include <sstream>

// ——— Constructor / Destructor ———

Database::Database(const std::string& dbFile)
  : db_(nullptr)
{
    if (sqlite3_open(dbFile.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "DB open error: " << sqlite3_errmsg(db_) << "\n";
        sqlite3_close(db_);
        db_ = nullptr;
    } else {
        initSchema();
    }
}

Database::~Database() {
    if (db_) sqlite3_close(db_);
}

// ——— run() / Analyse‐menu ———

void Database::run() {
    if (!db_) return;
    while (true) {
        showMenu();
        int choice = getChoice(1, 5);
        switch (choice) {
            case 1: listHeroesAlphabetical();    break;
            case 2: showTotalKillsPerHero();     break;
            case 3: showKillsByHeroPerWeapon();  break;
            case 4: showTopHeroPerWeapon();      break;
            case 5: return;                      // tilbage til hovedmenu
        }
        std::cout << "\nTryk Enter for at fortsætte…";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }
}

void Database::showMenu() const {
    std::cout << "\n=== Analyse-menu ===\n"
              << "1. Helte alfabetisk\n"
              << "2. Total kills per helt\n"
              << "3. Kills for én helt per våben\n"
              << "4. Top helt per våben\n"
              << "5. Tilbage\n"
              << "Valg: ";
}

int Database::getChoice(int min, int max) const {
    int c;
    while (!(std::cin >> c) || c < min || c > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Ugyldigt valg ("<<min<<"–"<<max<<"): ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return c;
}

void Database::listHeroesAlphabetical() const {
    std::cout << "\nHelte i alfabetisk rækkefølge:\n";
    executeSimpleQuery("SELECT name FROM heroes ORDER BY name;");
}

void Database::showTotalKillsPerHero() const {
    std::cout << "\nTotal kills per helt:\n";
    executeSimpleQuery(
      "SELECT hero, SUM(kills) AS total_kills "
      "FROM weapon_kills GROUP BY hero;"
    );
}

void Database::showKillsByHeroPerWeapon() const {
    std::cout << "\nIndtast heltenavn: ";
    std::string h; std::getline(std::cin, h);
    std::cout << "\nKills for " << h << " per våben:\n";

    sqlite3_stmt* stmt = nullptr;
    const char* sql =
      "SELECT weapon, kills FROM weapon_kills WHERE hero = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db_) << "\n";
        return;
    }
    sqlite3_bind_text(stmt, 1, h.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::cout << " - "
                  << sqlite3_column_text(stmt, 0)
                  << ": "
                  << sqlite3_column_int(stmt, 1) << "\n";
    }
    sqlite3_finalize(stmt);
}

void Database::showTopHeroPerWeapon() const {
    std::cout << "\nTop helt per våben:\n";
    executeSimpleQuery(
      "SELECT weapon, hero, kills "
      "FROM weapon_kills k "
      "WHERE kills = ("
      "  SELECT MAX(k2.kills) FROM weapon_kills k2 "
      "  WHERE k2.weapon = k.weapon"
      ");"
    );
}

// ——— Schema & CRUD ———

void Database::initSchema() {
    const char* sql = R"SQL(
      CREATE TABLE IF NOT EXISTS heroes (
        name TEXT PRIMARY KEY,
        xp INTEGER, level INTEGER,
        hp INTEGER, maxhp INTEGER,
        strength INTEGER, statPoints INTEGER,
        gold INTEGER
      );
      CREATE TABLE IF NOT EXISTS weapon_kills (
        hero TEXT, weapon TEXT,
        damage INTEGER, strengthModifier REAL,
        durability INTEGER, kills INTEGER,
        PRIMARY KEY(hero,weapon),
        FOREIGN KEY(hero) REFERENCES heroes(name)
      );
    )SQL";
    executeNonQuery(sql);
}

void Database::saveHero(const Hero& h) {
    std::ostringstream ss;
    ss << "INSERT OR REPLACE INTO heroes "
          "(name,xp,level,hp,maxhp,strength,statPoints,gold) VALUES ("
       << "'" << h.getName()   << "',"
       <<  h.getXp()          << ","
       <<  h.getLevel()       << ","
       <<  h.getHp()          << ","
       <<  h.getMaxHp()       << ","
       <<  h.getStrength()    << ","
       <<  h.getStatPoints()  << ","
       <<  h.getGold()        << ");";
    executeNonQuery(ss.str());
}

bool Database::loadHero(const std::string& name, Hero& outHero) {
    const char* sql =
      "SELECT xp,level,hp,maxhp,strength,statPoints,gold "
      "FROM heroes WHERE name = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        return false;
    }
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = sqlite3_step(stmt) == SQLITE_ROW;
    if (ok) {
        Hero tmp(
          name,
          sqlite3_column_int(stmt,0),
          sqlite3_column_int(stmt,1),
          sqlite3_column_int(stmt,2),
          sqlite3_column_int(stmt,3),
          sqlite3_column_int(stmt,4),
          sqlite3_column_int(stmt,5)
        );
        tmp.addGold(sqlite3_column_int(stmt,6));
        outHero = tmp;
    }
    sqlite3_finalize(stmt);
    return ok;
}

std::vector<std::string> Database::listHeroes() const {
    sqlite3_stmt* stmt = nullptr;
    std::vector<std::string> vec;
    if (sqlite3_prepare_v2(db_,
        "SELECT name FROM heroes;", -1, &stmt, nullptr) == SQLITE_OK)
    {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            vec.emplace_back(
              reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))
            );
        }
    }
    sqlite3_finalize(stmt);
    return vec;
}

void Database::saveWeaponKills(const Hero& h) {
    executeNonQuery(
      "DELETE FROM weapon_kills WHERE hero = '" + h.getName() + "';"
    );
    for (auto& w : h.getWeapons()) {
        std::ostringstream ss;
        ss << "INSERT INTO weapon_kills "
              "(hero,weapon,damage,strengthModifier,durability,kills) VALUES ("
           << "'" << h.getName()   << "',"
           << "'" << w.name        << "',"
           <<  w.damage           << ","
           <<  w.strengthModifier << ","
           <<  w.durability       << ","
           <<  w.kills            << ");";
        executeNonQuery(ss.str());
    }
}

std::vector<Weapon> Database::loadWeaponKills(const std::string& heroName) const {
    sqlite3_stmt* stmt = nullptr;
    std::vector<Weapon> vec;
    const char* sql =
      "SELECT weapon,damage,strengthModifier,durability,kills "
      "FROM weapon_kills WHERE hero = ?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt,1,heroName.c_str(),-1,SQLITE_TRANSIENT);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Weapon w;
            w.name             = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
            w.damage           = sqlite3_column_int(stmt,1);
            w.strengthModifier = sqlite3_column_double(stmt,2);
            w.durability       = sqlite3_column_int(stmt,3);
            w.kills            = sqlite3_column_int(stmt,4);
            vec.push_back(w);
        }
    }
    sqlite3_finalize(stmt);
    return vec;
}

// ——— Helper ———

void Database::executeNonQuery(const std::string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "[DB] SQL error: " << err << "\n";
        sqlite3_free(err);
    }
}

void Database::executeSimpleQuery(const std::string& sql) const {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int cols = sqlite3_column_count(stmt);
        for (int i = 0; i < cols; ++i) {
            const char* txt = reinterpret_cast<const char*>(sqlite3_column_text(stmt,i));
            std::cout << (txt?txt:"NULL") << (i<cols-1?" | ":"");
        }
        std::cout << "\n";
    }
    sqlite3_finalize(stmt);
}
