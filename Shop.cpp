// Shop.cpp
#include "Shop.h"
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>

Shop::Shop()
{
    // name, price, attackBonus
    items_.push_back({ "Jernsværd",    50,  3 });
    items_.push_back({ "Ståløkse",     75,  5 });
    items_.push_back({ "Langbue",     100,  4 });
    items_.push_back({ "Magisk stav", 150,  7 });
}

void Shop::displayItems() const
{
    std::cout << "\n=== Butik – Køb Våben ===\n";
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& it = items_[i];
        std::cout << (i + 1) << ". " << it.name
                  << " (Pris: " << it.price
                  << ", +"<< it.attackBonus <<" styrke)\n";
    }
    std::cout << (items_.size() + 1)
              << ". Tilbage\nValg: ";
}

void Shop::open(Hero& hero)
{
    while (true) {
        displayItems();

        int choice;
        // Input-validering inline
        while (true) {
            if (!(std::cin >> choice)
             || choice < 1
             || choice > static_cast<int>(items_.size()) + 1)
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Ugyldigt valg (1-"
                          << items_.size() + 1 << "): ";
            } else {
                break;
            }
        }

        // Tilbage
        if (choice == static_cast<int>(items_.size()) + 1) {
            break;
        }

        const auto& item = items_[choice - 1];
        if (hero.getGold() < item.price) {
            std::cout << "Ikke nok guld til " << item.name << ".\n";
        } else {
            hero.addGold(-item.price);
            hero.addWeapon(item.name, item.attackBonus);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
