#pragma once

#include "Items/Weapons/BasicWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Shield : public BasicWeapon
        {
        public:
            Shield(const ItemParameters& parameters, const std::string& nameCode, WeaponType type,
                   const std::string& skeletonPath, const std::string& matsetPath);
        };

        class WoodenBasicShield : public Shield
        {
        public:
            WoodenBasicShield(const ItemParameters& parameters);
        };

        class WoodenHeater : public Shield
        {
        public:
            WoodenHeater(const ItemParameters& parameters);
        };

        class BracedWoodenHeater : public Shield
        {
        public:
            BracedWoodenHeater(const ItemParameters& parameters);
        };

        class IronHeater : public Shield
        {
        public:
            IronHeater(const ItemParameters& parameters);
        };

        class WoodenRoundShield : public Shield
        {
        public:
            WoodenRoundShield(const ItemParameters& parameters);
        };

        class TowerShield : public Shield
        {
        public:
            TowerShield(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Shield>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WoodenBasicShield>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WoodenHeater>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BracedWoodenHeater>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::IronHeater>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WoodenRoundShield>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::TowerShield>(PreloadSet& preloads);
}
