#pragma once

#include "Items/Weapons/BasicWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BasicMeleeWeapon : public BasicWeapon
        {
        public:
            BasicMeleeWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath);
            BasicMeleeWeapon(const ItemParameters& parameters, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath);

            bool DoesSplashDamage() const override;

        protected:
            void SetSplash(bool doesSplash);

        private:
            bool _splash = false;
        };
    }

    template <>
    void EnumeratePreloads<Item::BasicMeleeWeapon>(PreloadSet& preloads);
}
