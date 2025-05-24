#pragma once
#include <string>

class Weapon {
private:
    std::string name_;
    int damage_;
    int strengthModifier_;
    int durability_;
public:
    Weapon(const std::string& name, int dmg, int strMod, int dur);
    const std::string& getName() const;
    int getDamage() const;
    int getStrengthModifier() const;
    int getDurability() const;
    int use(int heroStrength);
    bool isBroken() const;
};
