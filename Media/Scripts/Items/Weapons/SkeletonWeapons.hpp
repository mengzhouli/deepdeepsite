#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/Bow.hpp"

namespace Dwarf
{
    namespace Item
    {
        class SkelAxe : public BasicMeleeWeapon
        {
        public:
            SkelAxe(const ItemParameters& parameters);
        };

        class SkelDagger : public BasicMeleeWeapon
        {
        public:
            SkelDagger(const ItemParameters& parameters);
        };

        class SkelHalberd : public BasicMeleeWeapon
        {
        public:
            SkelHalberd(const ItemParameters& parameters);
        };

        class SkelScimitar : public BasicMeleeWeapon
        {
        public:
            SkelScimitar(const ItemParameters& parameters);
        };

        class SkelBow : public Bow
        {
        public:
            SkelBow(const ItemParameters& parameters);

        protected:
            virtual Character::Arrow* SpawnArrow(const Vector2f& position, const Character::Damage& dmg) override;
        };
    }

    template <>
    void EnumeratePreloads<Item::SkelAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::SkelDagger>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::SkelHalberd>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::SkelScimitar>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::SkelBow>(PreloadSet& preloads);
}
