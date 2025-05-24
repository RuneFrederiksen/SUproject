#include "Weapon.h"

Weapon::Weapon(const std::string& name, int dmg, int strMod, int dur)
  : name_(name), damage_(dmg), strengthModifier_(strMod), durability_(dur) {}

const std::string& Weapon::getName() const { return name_; }
int Weapon::getDamage() const { return damage_; }
int Weapon::getStrengthModifier() const { return strengthModifier_; }
int Weapon::getDurability() const { return durability_; }

int Weapon::use(int heroStrength) {
    if (durability_ <= 0) return 0;
    int total = damage_ + strengthModifier_ * heroStrength;
    durability_--;
    return total;
}

bool Weapon::isBroken() const { return durability_ <= 0; }
