// Database.cpp
#include "Database.h"
#include <iostream>
#include <limits>
#include <sstream>

Database::Database(const std::string& dbFile)
  : db_(nullptr), isOpen_(false)
{
    isOpen_ = openDatabase(dbFile);
    if (isOpen_) initSchema();
}

Database::~Database() {
    closeDatabase();
}

bool Database::openDatabase(const std::string& path) {
    if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db_) << "\n";
        return false;
    }
    return true;
}

void Database::closeDatabase() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void Database::initSchema() {
    const char* tables = R"SQL(
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
    char* err = nullptr;
    if (sqlite3_exec(db_, tables, nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "Schema init error: " << err << "\n";
        sqlite3_free(err);
    }
}

// ——— Hero CRUD ———

void Database::saveHero(const Hero& h) {
    const char* sql = R"SQL(
      INSERT OR REPLACE INTO heroes
        (name,xp,level,hp,maxhp,strength,statPoints,gold)
      VALUES(?, ?, ?, ?, ?, ?, ?, ?);
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "saveHero prepare failed: " << sqlite3_errmsg(db_) << "\n";
        return;
    }

    sqlite3_bind_text (stmt, 1, h.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int  (stmt, 2, h.getXp());
    sqlite3_bind_int  (stmt, 3, h.getLevel());
    sqlite3_bind_int  (stmt, 4, h.getHp());
    sqlite3_bind_int  (stmt, 5, h.getMaxHp());
    sqlite3_bind_int  (stmt, 6, h.getStrength());
    sqlite3_bind_int  (stmt, 7, h.getStatPoints());
    sqlite3_bind_int  (stmt, 8, h.getGold());

    if (sqlite3_step(stmt) != SQLITE_DONE)
        std::cerr << "saveHero exec failed: " << sqlite3_errmsg(db_) << "\n";

    sqlite3_finalize(stmt);
}

bool Database::loadHero(const std::string& name, Hero& outHero) {
    const char* sql = R"SQL(
      SELECT xp,level,hp,maxhp,strength,statPoints,gold
        FROM heroes WHERE name = ?;
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        Hero tmp(
          name,
          sqlite3_column_int(stmt,0),
          sqlite3_column_int(stmt,1),
          sqlite3_column_int(stmt,2),
          sqlite3_column_int(stmt,3),
          sqlite3_column_int(stmt,4),
          sqlite3_column_int(stmt,5)
        );
        // gold is separate
        tmp.addGold(sqlite3_column_int(stmt,6));
        outHero = std::move(tmp);
        ok = true;
    }
    sqlite3_finalize(stmt);
    return ok;
}

std::vector<std::string> Database::listHeroes() const {
    const char* sql = "SELECT name FROM heroes;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<std::string> result;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            result.emplace_back(
              reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))
            );
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

// ——— Weapon‐kills CRUD ———

void Database::saveWeaponKills(const Hero& h) {
    // Delete existing
    {
      const char* del = "DELETE FROM weapon_kills WHERE hero = ?;";
      sqlite3_stmt* stmt = nullptr;
      sqlite3_prepare_v2(db_, del, -1, &stmt, nullptr);
      sqlite3_bind_text(stmt,1,h.getName().c_str(),-1,SQLITE_TRANSIENT);
      sqlite3_step(stmt);
      sqlite3_finalize(stmt);
    }

    // Insert each
    const char* ins = R"SQL(
      INSERT INTO weapon_kills
      (hero,weapon,damage,strengthModifier,durability,kills)
      VALUES(?, ?, ?, ?, ?, ?);
    )SQL";
    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, ins, -1, &stmt, nullptr);

    for (auto& w : h.getWeapons()) {
        sqlite3_reset(stmt);
        sqlite3_bind_text   (stmt, 1, h.getName().c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text   (stmt, 2, w.name.c_str(),     -1, SQLITE_TRANSIENT);
        sqlite3_bind_int    (stmt, 3, w.damage);
        sqlite3_bind_double (stmt, 4, w.strengthModifier);
        sqlite3_bind_int    (stmt, 5, w.durability);
        sqlite3_bind_int    (stmt, 6, w.kills);

        if (sqlite3_step(stmt) != SQLITE_DONE)
            std::cerr << "saveWeaponKills exec failed: " << sqlite3_errmsg(db_) << "\n";
    }
    sqlite3_finalize(stmt);
}

std::vector<Weapon> Database::loadWeaponKills(const std::string& heroName) const {
    const char* sql = R"SQL(
      SELECT weapon,damage,strengthModifier,durability,kills
        FROM weapon_kills WHERE hero = ?;
    )SQL";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Weapon> result;

    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr)==SQLITE_OK) {
        sqlite3_bind_text(stmt,1,heroName.c_str(),-1,SQLITE_TRANSIENT);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Weapon w;
            w.name             = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
            w.damage           = sqlite3_column_int(stmt,1);
            w.strengthModifier = sqlite3_column_double(stmt,2);
            w.durability       = sqlite3_column_int(stmt,3);
            w.kills            = sqlite3_column_int(stmt,4);
            result.push_back(std::move(w));
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

// ——— Analysis menu ———

void Database::run() {
    if (!isOpen_) return;
    while (true) {
        showMenu();
        int choice = getChoice(1,5);
        switch (choice) {
          case 1: listHeroesAlphabetical();    break;
          case 2: showTotalKillsPerHero();     break;
          case 3: showKillsByHeroPerWeapon();  break;
          case 4: showTopHeroPerWeapon();      break;
          case 5: return;
        }
        std::cout << "\nPress Enter…";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
}

void Database::showMenu() const {
    std::cout<<"\n=== Analyse ===\n"
             <<"1) Helte alfabetisk\n"
             <<"2) Total kills per helt\n"
             <<"3) Kills per våben for helt\n"
             <<"4) Top helt per våben\n"
             <<"5) Tilbage\n"
             <<"Valg: ";
}

int Database::getChoice(int min, int max) const {
    int c;
    while (!(std::cin>>c) || c<min||c>max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::cout<<"Ugyldigt valg ("<<min<<"–"<<max<<"): ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    return c;
}

void Database::listHeroesAlphabetical() const {
    std::cout<<"\nHelte alfabetisk:\n";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,"SELECT name FROM heroes ORDER BY name;",-1,&stmt,nullptr);
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)<<"\n";
    sqlite3_finalize(stmt);
}

void Database::showTotalKillsPerHero() const {
    std::cout<<"\nTotal kills per helt:\n";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      "SELECT hero, SUM(kills) FROM weapon_kills GROUP BY hero;",
      -1,&stmt,nullptr);
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<" : "<<sqlite3_column_int(stmt,1)<<"\n";
    sqlite3_finalize(stmt);
}

void Database::showKillsByHeroPerWeapon() const {
    std::cout<<"\nIndtast heltenavn: ";
    std::string h; std::getline(std::cin,h);
    std::cout<<"\nKills for "<<h<<":\n";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      "SELECT weapon,kills FROM weapon_kills WHERE hero=?;",-1,&stmt,nullptr);
    sqlite3_bind_text(stmt,1,h.c_str(),-1,SQLITE_TRANSIENT);
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<" : "<<sqlite3_column_int(stmt,1)<<"\n";
    sqlite3_finalize(stmt);
}

void Database::showTopHeroPerWeapon() const {
    std::cout<<"\nTop helt per våben:\n";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      R"SQL(
        SELECT k.weapon, k.hero, k.kills
          FROM weapon_kills k
         WHERE k.kills = (
           SELECT MAX(k2.kills) FROM weapon_kills k2
            WHERE k2.weapon = k.weapon
         );
      )SQL", -1, &stmt,nullptr);
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<" : "<<sqlite3_column_text(stmt,1)
                 <<" ("<<sqlite3_column_int(stmt,2)<<" kills)\n";
    sqlite3_finalize(stmt);
}
