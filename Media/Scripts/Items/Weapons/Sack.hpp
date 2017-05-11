#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Sack : public BasicMeleeWeapon, public MinerSack
        {
        public:
            Sack(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Sack>(PreloadSet& preloads);
}
