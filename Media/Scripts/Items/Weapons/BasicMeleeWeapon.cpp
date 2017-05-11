#include "Items/Weapons/BasicMeleeWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        BasicMeleeWeapon::BasicMeleeWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicWeapon(parameters, nameCode, type, skeletonPath, matsetPath)
        {
        }

        BasicMeleeWeapon::BasicMeleeWeapon(const ItemParameters& parameters, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicMeleeWeapon(parameters, "", type, skeletonPath, matsetPath)
        {
        }

        bool BasicMeleeWeapon::DoesSplashDamage() const
        {
            return _splash;
        }

        void BasicMeleeWeapon::SetSplash(bool doesSplash)
        {
            _splash = doesSplash;
        }
    }

    template <>
    void EnumeratePreloads<Item::BasicMeleeWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicWeapon>(preloads);
    }
}
