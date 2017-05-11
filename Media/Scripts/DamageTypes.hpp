#pragma once

#include "Character/CharacterTypes.hpp"

namespace Dwarf
{
    namespace Character
    {
        // Various distances the damage comes from
        const DamageType DamageType_Type_Melee = 1 << 0;
        const DamageType DamageType_Type_Projectile = 1 << 1;
        const DamageType DamageType_Type_Fall = 1 << 3;
        const DamageType DamageType_Type_Explosion = 1 << 4;

        // Damage sources
        const DamageType DamageType_Source_Slashing = 1 << 17;
        const DamageType DamageType_Source_Crushing = 1 << 18;
        const DamageType DamageType_Source_Piercing = 1 << 19;
        const DamageType DamageType_Source_Magic = 1 << 20;
        const DamageType DamageType_Source_Explosion = 1 << 21;

        // Damage elements
        const DamageType DamageType_Element_Physical = 1 << 24;
        const DamageType DamageType_Element_Fire = 1 << 25;
        const DamageType DamageType_Element_Poison = 1 << 26;

        // Body materials
        const MaterialType MaterialType_Type_Flesh = 1 << 0;
        const MaterialType MaterialType_Type_Cloth = 1 << 1;
        const MaterialType MaterialType_Type_Metal = 1 << 2;
        const MaterialType MaterialType_Type_Bone = 1 << 3;
    }
}
