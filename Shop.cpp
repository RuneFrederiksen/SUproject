#include "Shop.h"
#include "Hero.h"      
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

Shop::Shop()
{
    // name, price, damage, strengthModifier, durability
    items_.push_back({ "sværd",    50,  5,  0.2,  65 });
    items_.push_back({ "økse",     75,  7,  0.1,  70 });
    items_.push_back({ "bue",     100,  4,  0.5,   60 });
    items_.push_back({ "stav", 150,  3,  1.0,  50 });
    items_.push_back({ "bazooka", 1000,  100,  0,  1 });
}

void Shop::displayItems() const
{
    std::cout << "\n=== Butik – Køb Våben ===\n";
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& it = items_[i];
        std::cout 
            << (i + 1) << ". " << it.name
            << " (Pris: " << it.price << " guld, "
            << "Skade: " << it.damage << ", "
            << "Mod.: x" << it.strengthModifier << ", "
            << "Holdbarhed: " << it.durability << ")\n";
    }
    std::cout << (items_.size() + 1)
              << ". Tilbage\nValg: ";
}

void Shop::open(Hero& hero)
{
    while (true) {
        displayItems();

        int choice;
        // Input‐validering inline
        while (!(std::cin >> choice) ||
               choice < 1 ||
               choice > static_cast<int>(items_.size()) + 1)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ugyldigt valg (1-" 
                      << items_.size() + 1 << "): ";
        }

        if (choice == static_cast<int>(items_.size()) + 1)
            break;  // tilbage til menu

        const auto& item = items_[choice - 1];
        if (hero.getGold() < item.price) {
            std::cout << "Ikke nok guld til " << item.name << ".\n";
        } else {
            hero.addGold(-item.price);

            // Her laver vi en Weapon og kalder den ene addWeapon(Weapon&)
            Weapon w;
            w.name             = item.name;
            w.damage           = item.damage;
            w.strengthModifier = item.strengthModifier;
            w.durability       = item.durability;
            w.kills            = 0;

            hero.addWeapon(w);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
