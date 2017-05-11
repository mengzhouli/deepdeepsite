#pragma once

#include "Items/Trinkets/BasicTrinket.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Cookbook : public BasicTrinket, public ChannelingFoodModifier
        {
        public:
            Cookbook(const ItemParameters& parameters, const std::string nameCode, const std::string& matsetPath);

            float GetCookRangeModifier() const override;
            uint32_t GetCookFoodAdditionalTargetCount() const override;
            bool CookedFoodDoesAOE() const override;
            bool CookedFoodTargetsEnemies() const override;
            bool CookedFoodHealsOverTime() const override;

        protected:
            virtual void OnEquip(Character::Character* newOwner) override;
        };

        class DeVermisCoctioneCookbook : public Cookbook
        {
        public:
            DeVermisCoctioneCookbook(const ItemParameters& parameters);

            bool CookedFoodHealsOverTime() const override;
        };

        class TheChefInYellowCookbook : public Cookbook
        {
        public:
            TheChefInYellowCookbook(const ItemParameters& parameters);

            uint32_t GetCookFoodAdditionalTargetCount() const override;
        };

        class CulinomiconCookbook : public Cookbook
        {
        public:
            CulinomiconCookbook(const ItemParameters& parameters);

            bool CookedFoodTargetsEnemies() const override;
        };
    }

    template <>
    void EnumeratePreloads<Item::Cookbook>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::DeVermisCoctioneCookbook>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::TheChefInYellowCookbook>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::CulinomiconCookbook>(PreloadSet& preloads);
}
