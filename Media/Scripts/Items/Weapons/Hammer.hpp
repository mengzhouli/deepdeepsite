#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Hammer : public BasicMeleeWeapon, public BuildRateModifier
        {
        public:
            Hammer(const ItemParameters& parameters, const std::string& name, float buildMult, const std::string& matsetPath);

            virtual float GetBuildRateMultiplier() const override;

        private:
            float _buildRateMult;
        };

        class WoodHammer : public Hammer
        {
        public:
            WoodHammer(const ItemParameters& parameters);
        };

        class IronHammer : public Hammer
        {
        public:
            IronHammer(const ItemParameters& parameters);
        };

        class GoldHammer : public Hammer
        {
        public:
            GoldHammer(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Hammer>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WoodHammer>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::IronHammer>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GoldHammer>(PreloadSet& preloads);
}
