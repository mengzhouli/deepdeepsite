#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class OrkWeapon : public BasicMeleeWeapon
        {
        public:
            OrkWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath);

            void OnPostInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& damage) override;

        private:
            bool _doesKnockback;
        };

        class OrkChopper : public OrkWeapon
        {
        public:
            OrkChopper(const ItemParameters& parameters);
        };

        class OrkClub : public OrkWeapon
        {
        public:
            OrkClub(const ItemParameters& parameters);
        };

        class OrkHook : public OrkWeapon
        {
        public:
            OrkHook(const ItemParameters& parameters);
        };

        class OrkMace : public OrkWeapon
        {
        public:
            OrkMace(const ItemParameters& parameters);
        };

        class OrkLance : public OrkWeapon
        {
        public:
            OrkLance(const ItemParameters& parameters);
        };

        class OrkTowerShield : public BasicWeapon
        {
        public:
            OrkTowerShield(const ItemParameters& parameters);

            bool DoesSplashDamage() const override;

            uint32_t GetBreakablePieceCount() const;
            void BreakNextPiece();

        private:
            uint32_t _curBreakPiece;
        };
    }

    template <>
    void EnumeratePreloads<Item::OrkWeapon>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkChopper>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkClub>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkHook>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkMace>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkLance>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::OrkTowerShield>(PreloadSet& preloads);
}
