#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class WoodenShortSword : public BasicMeleeWeapon
        {
        public:
            WoodenShortSword(const ItemParameters& parameters);
        };

        class RustyIronShortSword : public BasicMeleeWeapon
        {
        public:
            RustyIronShortSword(const ItemParameters& parameters);
        };

        class IronShortSword : public BasicMeleeWeapon
        {
        public:
            IronShortSword(const ItemParameters& parameters);
        };

        class IgnitionPinSword : public BasicMeleeWeapon
        {
        public:
            IgnitionPinSword(const ItemParameters& parameters);
        };

        class LettersbaneSword : public BasicMeleeWeapon
        {
        public:
            LettersbaneSword(const ItemParameters& parameters);
        };

        class UncalculatedDivisionSword : public BasicMeleeWeapon
        {
        public:
            UncalculatedDivisionSword(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::WoodenShortSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::RustyIronShortSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::IronShortSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::IgnitionPinSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::LettersbaneSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::UncalculatedDivisionSword>(PreloadSet& preloads);
}
