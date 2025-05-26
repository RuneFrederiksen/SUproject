#ifndef SHOP_H
#define SHOP_H

#include "Hero.h"
#include <vector>
#include <string>

struct ShopItem {
    std::string name;
    int price;
    int damage;               // base skade
    double strengthModifier;  // fx 0.5 betyder +50% af hero.strength
    int durability;           // start‐holdbarhed
};

class Shop {
public:
    Shop();
    void open(Hero& hero);

private:
    void displayItems() const;
    std::vector<ShopItem> items_;
};

#endif
