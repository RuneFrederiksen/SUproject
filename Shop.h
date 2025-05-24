// Shop.h
#ifndef SHOP_H
#define SHOP_H

#include "Hero.h"
#include <vector>
#include <string>

struct ShopItem {
    std::string name;
    int price;
    int attackBonus;
};

class Shop {
public:
    Shop();
    void open(Hero& hero);

private:
    void displayItems() const;
    std::vector<ShopItem> items_;
};

#endif // SHOP_H
