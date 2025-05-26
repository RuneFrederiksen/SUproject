#include "GameEngine.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

// Constructor
GameEngine::GameEngine(const std::string& dbFile)
  : hero_(nullptr)         // starter uden en aktiv helt
  , grotte_()              // grottemodulet
  , shop_()                // butikmodulet
  , db_(dbFile)            // database (opretter .db og schema om nødvendigt)
{}


void GameEngine::run() {
    while (true) {
        showMainMenu();
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
}

// Hovedmenu med valg for nyt spil, load, save+exit, analyse eller exit uden save
void GameEngine::showMainMenu() {
    std::cout << "\n=== Hovedmenu ===\n"
              << "1. New Game\n"
              << "2. Load Game\n"
              << "3. Save & Exit\n"
              << "4. Analyse\n"
              << "5. Exit Without Saving\n"
              << "Choice: ";
    int choice = getChoice(1, 5);

    switch (choice) {
        case 1: newGame();      break;
        case 2: loadGame();     break;
        case 3: saveAndExit();  break;
        case 4: db_.runAnalysisMenu(); break;   // åbner analyse-menuen
        case 5:
        default: std::exit(0);           // lukker programmet uden at gemme
    }
}

// Starter et nyt spil: beder om navn, opretter helten og gemmer i DB
void GameEngine::newGame() {
    std::cout << "Enter hero name: ";
    std::string name;
    std::cin >> name;

    hero_ = std::make_unique<Hero>(name);
    // Gem starttilstand
    db_.saveHero(*hero_);
    db_.saveWeaponStats(*hero_);

    // Kør adventure-menu indtil helten dør
    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }
    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Loader en eksisterende helt fra databasen og kører adventure-menu
void GameEngine::loadGame() {
    auto names = db_.getAllHeroNames();
    if (names.empty()) {
        std::cout << "Ingen helte fundet.\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return;
    }
    std::cout << "Vælg en helt:\n";
    for (size_t i = 0; i < names.size(); ++i) {
        std::cout << (i+1) << ". " << names[i] << "\n";
    }
    int idx = getChoice(1, static_cast<int>(names.size()));

    // Midlertidig hero til load
    Hero tmp("",0,0,0,0,0,0);
    if (!db_.loadHero(names[idx-1], tmp)) {
        std::cout << "Load fejlede for " << names[idx-1] << ".\n";
        return;
    }

    // Overfør til smart-pointer og hent våben-statistikker
    hero_ = std::make_unique<Hero>(tmp);
    for (auto& w : db_.loadWeaponStats(tmp.getName())) {
        hero_->addWeapon(w);
    }

    while (hero_ && hero_->getHp() > 0) {
        showAdventureMenu();
    }
    std::cout << "Game Over.\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

// Adventure-menu: vis stats, valg om level up, heal, grottekørsel, butik, inventory eller tilbage
void GameEngine::showAdventureMenu() {
    // Vis helte-stats
    std::cout << "\n[ " << hero_->getName()
              << " — HP: "   << hero_->getHp()   << "/" << hero_->getMaxHp()
              << ", Skade: " << hero_->getStrength()
              << ", XP: "    << hero_->getXp()
              << ", Guld: "  << hero_->getGold()
              << " ]\n";

    // Menupunkter
    std::cout << "\n=== Hvad vil du gøre? ===\n"
              << "1. Level Up\n"
              << "2. Heal Up\n"
              << "3. Gå ind i grotten\n"
              << "4. Åbn Butik\n"
              << "5. Vis Inventory\n"
              << "6. Tilbage til Hovedmenu\n"
              << "Choice: ";
    int choice = getChoice(1, 6);

    switch (choice) {
        case 1:
            manualLevelUp();
            break;
        case 2:
            // Heal trækker XP og fylder HP
            hero_->restoreHp();
            std::cout << hero_->getName()
                      << " er fuldt helbredt (-100 XP)\n";
            break;
        case 3:
            // Gå i grotten: kør kampene, gem efterfølgende
            grotte_.enter(*hero_);
            db_.saveHero(*hero_);
            db_.saveWeaponStats(*hero_);
            break;
        case 4:
            shop_.open(*hero_);
            break;
        case 5:
            // Fjern evt. tilbageværende '\n' så showWeapons() kan vente på input
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            hero_->showWeapons();
            break;
        case 6:
            // Tilbage til hovedmenu
            showMainMenu();
            return;
        default:
            return;
    }

    // Kort pause før næste loop
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

// Manuel level-up: tjek om mulig, gennemfør og tildel stats-points
void GameEngine::manualLevelUp() {
    if (!hero_->canLevelUp()) {
        std::cout << "Du kan ikke level up nu.\n";
        return;
    }
    hero_->levelUp();
    std::cout << "Tillykke! Nu niveau " << hero_->getLevel() << ".\n";
    allocateStats();
}

// Fordel stat-points mellem HP og styrke
void GameEngine::allocateStats() {
    while (hero_->getStatPoints() > 0) {
        std::cout << "\nDu har " << hero_->getStatPoints()
                  << " stat points.\n"
                  << "1. HP  \n2. Styrke  \n3. Færdig\n"
                  << "Choice: ";
        int c = getChoice(1, 3);
        if (c == 3) break;

        std::cout << "Hvor mange? ";
        int pts = getChoice(0, hero_->getStatPoints());
        if (c == 1) hero_->allocateStats(pts, 0);
        else        hero_->allocateStats(0, pts);
    }
}

// Gem spil og afslut program
void GameEngine::saveAndExit() {
    if (!hero_) {
        std::cout << "Intet spil at gemme.\n";
    } else {
        db_.saveHero(*hero_);
        db_.saveWeaponStats(*hero_);
        std::cout << "Spillet er gemt. Afslutter...\n";
    }
    std::exit(0);
}

// Hjælpe-funktion til at validere brugerinput (valg mellem min-max)
int GameEngine::getChoice(int min, int max) const {
    int c;
    while (!(std::cin >> c) || c < min || c > max) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                        '\n');
        std::cout << "Ugyldigt valg (" << min << "-" << max << "): ";
    }
    return c;
}
