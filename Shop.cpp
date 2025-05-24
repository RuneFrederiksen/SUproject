// Shop.cpp
#include "Shop.h"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

Shop::Shop()
{
    // name, price, damage, strengthModifier, durability
    items_.push_back({ "Jernsværd",    50,  5,  0.2,  10 });
    items_.push_back({ "Ståløkse",     75,  7,  0.1,  12 });
    items_.push_back({ "Langbue",     100,  4,  0.5,   8 });
    items_.push_back({ "Magisk stav", 150,  3,  1.0,  15 });
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
            // hero::addWeapon skal tage (name, damage, strengthModifier, durability)
            hero.addWeapon(item.name,
                           item.damage,
                           item.strengthModifier,
                           item.durability);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
