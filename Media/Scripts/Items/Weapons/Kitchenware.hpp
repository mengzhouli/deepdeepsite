#pragma once

#include "Items/Weapons/ThrowingWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Cutlery : public ThrowingWeapon
        {
        public:
            Cutlery(const ItemParameters& parameters);
        };

        class ServingTools : public ThrowingWeapon
        {
        public:
            ServingTools(const ItemParameters& parameters);

        protected:
            void OnEquip(Character::Character* newOwner) override;
            void OnUnEquip(Character::Character* oldOwner) override;
        };

        class PrepTools : public ThrowingWeapon
        {
        public:
            PrepTools(const ItemParameters& parameters);

        protected:
            void OnPostInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& damage);
        };

        class ButcherTools : public ThrowingWeapon
        {
        public:
            ButcherTools(const ItemParameters& parameters);
        };

        class Cookware : public ThrowingWeapon
        {
        public:
            Cookware(const ItemParameters& parameters);
        };

        class AllKitchenware : public ThrowingWeapon
        {
        public:
            AllKitchenware(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Cutlery>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::ServingTools>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::PrepTools>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::ButcherTools>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::Cookware>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::AllKitchenware>(PreloadSet& preloads);
}
