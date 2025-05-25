#include "Database.h"
#include <iostream>
#include <sstream>
#include <limits>

DBManager::DBManager(const std::string& filename)
    : db_(nullptr), dbFile_(filename)
{
    if (openConnection()) {
        ensureSchema();
    }
}

DBManager::~DBManager() {
    closeConnection();
}

bool DBManager::openConnection() {
    if (sqlite3_open(dbFile_.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "[DB] Cannot open " << dbFile_
                  << ": " << sqlite3_errmsg(db_) << "\n";
        db_ = nullptr;
        return false;
    }
    return true;
}

void DBManager::closeConnection() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void DBManager::ensureSchema() {
    const char* ddl = R"SQL(
      CREATE TABLE IF NOT EXISTS heroes (
        name TEXT PRIMARY KEY,
        xp INTEGER, level INTEGER,
        hp INTEGER, maxhp INTEGER,
        strength INTEGER, statPoints INTEGER,
        gold INTEGER
      );
      CREATE TABLE IF NOT EXISTS weapon_kills (
        hero TEXT,
        weapon TEXT,
        damage INTEGER,
        strengthModifier REAL,
        durability INTEGER,
        kills INTEGER,
        PRIMARY KEY(hero,weapon),
        FOREIGN KEY(hero) REFERENCES heroes(name)
      );
    )SQL";
    char* err = nullptr;
    sqlite3_exec(db_, ddl, nullptr, nullptr, &err);
    if (err) {
        std::cerr << "[DB] Schema error: " << err << "\n";
        sqlite3_free(err);
    }
}

// ——— Hero persistence ———

void DBManager::saveHero(const Hero& h) {
    const char* sql = R"SQL(
      INSERT OR REPLACE INTO heroes
        (name,xp,level,hp,maxhp,strength,statPoints,gold)
      VALUES (?, ?, ?, ?, ?, ?, ?, ?);
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text (stmt, 1, h.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int  (stmt, 2, h.getXp());
    sqlite3_bind_int  (stmt, 3, h.getLevel());
    sqlite3_bind_int  (stmt, 4, h.getHp());
    sqlite3_bind_int  (stmt, 5, h.getMaxHp());
    sqlite3_bind_int  (stmt, 6, h.getStrength());
    sqlite3_bind_int  (stmt, 7, h.getStatPoints());
    sqlite3_bind_int  (stmt, 8, h.getGold());

    if (sqlite3_step(stmt) != SQLITE_DONE)
        std::cerr << "[DB] saveHero failed: " << sqlite3_errmsg(db_) << "\n";
    sqlite3_finalize(stmt);
}

bool DBManager::loadHero(const std::string& name, Hero& outHero) {
    const char* sql = R"SQL(
      SELECT xp,level,hp,maxhp,strength,statPoints,gold
        FROM heroes WHERE name = ?;
    )SQL";

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool found = false;
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
        tmp.addGold(sqlite3_column_int(stmt,6));
        outHero = std::move(tmp);
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

std::vector<std::string> DBManager::getAllHeroNames() const {
    const char* sql = "SELECT name FROM heroes;";
    sqlite3_stmt* stmt = nullptr;
    std::vector<std::string> list;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.emplace_back(
          reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))
        );
    }
    sqlite3_finalize(stmt);
    return list;
}

// ——— Weapon‐kill stats ———

void DBManager::saveWeaponStats(const Hero& h) {
    // remove old
    {
      const char* del = "DELETE FROM weapon_kills WHERE hero = ?;";
      sqlite3_stmt* stmt = nullptr;
      sqlite3_prepare_v2(db_, del, -1, &stmt, nullptr);
      sqlite3_bind_text(stmt,1,h.getName().c_str(),-1,SQLITE_TRANSIENT);
      sqlite3_step(stmt);
      sqlite3_finalize(stmt);
    }
    // insert current
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
            std::cerr << "[DB] saveWeaponStats failed: " << sqlite3_errmsg(db_)<<"\n";
    }
    sqlite3_finalize(stmt);
}

std::vector<Weapon> DBManager::loadWeaponStats(const std::string& heroName) const {
    const char* sql = R"SQL(
      SELECT weapon,damage,strengthModifier,durability,kills
        FROM weapon_kills WHERE hero = ?;
    )SQL";
    sqlite3_stmt* stmt = nullptr;
    std::vector<Weapon> result;
    sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
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
    sqlite3_finalize(stmt);
    return result;
}

// ——— Analysis menu ———

void DBManager::runAnalysisMenu() {
    if (!db_) return;
    while (true) {
        showAnalysisOptions();
        int choice = readChoice(1,5);
        switch (choice) {
          case 1: listHeroesAlpha();    break;
          case 2: reportTotalKills();    break;
          case 3: reportKillsByWeapon(); break;
          case 4: reportTopHeroPerWeapon(); break;
          case 5: return;
        }
        std::cout << "\n(tryk Enter for at fortsætte) ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
}

void DBManager::showAnalysisOptions() const {
    std::cout<<"\n-- Analyse --\n"
             <<"1) Helte alfabetisk\n"
             <<"2) Total kills per helt\n"
             <<"3) Kills per våben\n"
             <<"4) Top per våben\n"
             <<"5) Tilbage\n"
             <<"Valg: ";
}

int DBManager::readChoice(int min, int max) const {
    int c;
    while (!(std::cin>>c) || c<min||c>max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        std::cout<<"Ugyldigt ("<<min<<"-"<<max<<"): ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    return c;
}

void DBManager::listHeroesAlpha() const {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,"SELECT name FROM heroes ORDER BY name;",-1,&stmt,nullptr);
    std::cout<<"\nHelte:\n";
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)<<"\n";
    sqlite3_finalize(stmt);
}

void DBManager::reportTotalKills() const {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      "SELECT hero, SUM(kills) FROM weapon_kills GROUP BY hero;",-1,&stmt,nullptr);
    std::cout<<"\nTotal kills per helt:\n";
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<": "<<sqlite3_column_int(stmt,1)<<"\n";
    sqlite3_finalize(stmt);
}

void DBManager::reportKillsByWeapon() const {
    std::cout<<"\nHeltnavn: ";
    std::string h; std::getline(std::cin,h);
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      "SELECT weapon,kills FROM weapon_kills WHERE hero=?;",-1,&stmt,nullptr);
    sqlite3_bind_text(stmt,1,h.c_str(),-1,SQLITE_TRANSIENT);
    std::cout<<"\nKills for "<<h<<":\n";
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<": "<<sqlite3_column_int(stmt,1)<<"\n";
    sqlite3_finalize(stmt);
}

void DBManager::reportTopHeroPerWeapon() const {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_,
      R"SQL(
        SELECT w.weapon, w.hero, w.kills
          FROM weapon_kills w
         WHERE w.kills = (
           SELECT MAX(x.kills) FROM weapon_kills x
            WHERE x.weapon = w.weapon
         );
      )SQL", -1, &stmt,nullptr);
    std::cout<<"\nTop per våben:\n";
    while (sqlite3_step(stmt)==SQLITE_ROW)
        std::cout<<" - "<<sqlite3_column_text(stmt,0)
                 <<": "<<sqlite3_column_text(stmt,1)
                 <<" ("<<sqlite3_column_int(stmt,2)<<" kills)\n";
    sqlite3_finalize(stmt);
}
