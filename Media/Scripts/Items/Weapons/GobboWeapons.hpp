#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/Bow.hpp"
#include "Items/Weapons/ThrowingWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class GobBlade : public BasicMeleeWeapon
        {
        public:
            GobBlade(const ItemParameters& parameters);
        };

        class GobDagger : public BasicMeleeWeapon
        {
        public:
            GobDagger(const ItemParameters& parameters);
        };

        class GobFork : public BasicMeleeWeapon
        {
        public:
            GobFork(const ItemParameters& parameters);
        };

        class GobShank : public BasicMeleeWeapon
        {
        public:
            GobShank(const ItemParameters& parameters);
        };

        class GobSword : public BasicMeleeWeapon
        {
        public:
            GobSword(const ItemParameters& parameters);
        };

        class GobTrident : public BasicMeleeWeapon
        {
        public:
            GobTrident(const ItemParameters& parameters);
        };

        class GobJawBlade : public BasicMeleeWeapon
        {
        public:
            GobJawBlade(const ItemParameters& parameters);
        };

        class GobSkullClub : public BasicMeleeWeapon
        {
        public:
            GobSkullClub(const ItemParameters& parameters);
        };

        class GobOrkAxe : public BasicMeleeWeapon
        {
        public:
            GobOrkAxe(const ItemParameters& parameters);
        };

        class GobOrkSickle : public BasicMeleeWeapon
        {
        public:
            GobOrkSickle(const ItemParameters& parameters);
        };

        class GobBow : public Bow
        {
        public:
            GobBow(const ItemParameters& parameters);

        protected:
            virtual Character::Arrow* SpawnArrow(const Vector2f& position, const Character::Damage& dmg) override;
        };

        class GobThrowingRocks : public ThrowingWeapon
        {
        public:
            GobThrowingRocks(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::GobBlade>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobDagger>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobFork>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobShank>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobSword>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobTrident>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobJawBlade>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobSkullClub>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobOrkAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobOrkSickle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobBow>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GobThrowingRocks>(PreloadSet& preloads);
}
