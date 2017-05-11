#pragma once

#include "Characters/BasicCharacter.hpp"
#include "Items/Weapons/BasicMeleeWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class GnomeWrench : public BasicMeleeWeapon
        {
        public:
            GnomeWrench(const ItemParameters& parameters);
        };
    }

    namespace Character
    {
        class Gnome : public BasicCharacter
        {
        public:
            Gnome(const CharacterParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::GnomeWrench>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::Gnome>(PreloadSet& preloads);
}
