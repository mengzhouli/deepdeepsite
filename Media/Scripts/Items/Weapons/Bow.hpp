#pragma once

#include "Items/Weapons/BasicRangedWeapon.hpp"
#include "Characters/Arrow.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Bow : public BasicRangedWeapon
        {
        public:
            Bow(const ItemParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath);

        protected:
            virtual void Fire(const Character::Damage& dmg) override;
            virtual Character::Arrow* SpawnArrow(const Vector2f& position, const Character::Damage& dmg) = 0;
        };
    }

    template <>
    void EnumeratePreloads<Item::Bow>(PreloadSet& preloads);
}
