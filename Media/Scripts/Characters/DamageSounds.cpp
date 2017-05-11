#include "Characters/DamageSounds.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Audio
    {
        static const std::vector<DamageSoundPaths> StandardOnFleshDamageSoundPaths =
        {
            // On-flesh sounds
            {
                Character::MaterialType_Type_Flesh,
                Character::DamageType_Source_Slashing,
                {
                    { "Audio/Characters/Damage/damage_slash_FLESHier1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_FLESHier2.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_FLESHier3.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Flesh,
                Character::DamageType_Source_Piercing,
                {
                    { "Audio/Characters/Damage/damage_pierce_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_flesh2.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_flesh3.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Flesh,
                Character::DamageType_Source_Crushing,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Flesh,
                Character::DamageType_Source_Explosion,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
        };

        static const std::vector<DamageSoundPaths> StandardOnClothDamageSoundPaths =
        {
            // On-flesh sounds
            {
                Character::MaterialType_Type_Cloth,
                Character::DamageType_Source_Slashing,
                {
                    { "Audio/Characters/Damage/damage_slash_FLESHier1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_FLESHier2.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_FLESHier3.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Cloth,
                Character::DamageType_Source_Piercing,
                {
                    { "Audio/Characters/Damage/damage_pierce_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_flesh2.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_flesh3.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Cloth,
                Character::DamageType_Source_Crushing,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Cloth,
                Character::DamageType_Source_Explosion,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
        };

        static const std::vector<DamageSoundPaths> StandardOnMetalDamageSoundPaths =
        {
            // On-armor sounds
            {
                Character::MaterialType_Type_Metal,
                Character::DamageType_Source_Slashing,
                {
                    { "Audio/Characters/Damage/damage_slash_armour1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_armour2.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Metal,
                Character::DamageType_Source_Piercing,
                {
                    { "Audio/Characters/Damage/damage_pierce_armour1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_armour2.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Metal,
                Character::DamageType_Source_Crushing,
                {
                    { "Audio/Characters/Damage/damage_crush_armour1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_armour2.wav", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Metal,
                Character::DamageType_Source_Explosion,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
        };

        static const std::vector<DamageSoundPaths> StandardOnBoneDamageSoundPaths =
        {
            // On-armor sounds
            {
                Character::MaterialType_Type_Bone,
                Character::DamageType_Source_Slashing,
                {
                    { "Audio/Characters/Damage/damage_slash_skeleton1.ogg", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_skeleton2.ogg", 1.0f },
                    { "Audio/Characters/Damage/damage_slash_skeleton3.ogg", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Bone,
                Character::DamageType_Source_Piercing,
                {
                    { "Audio/Characters/Damage/damage_pierce_skeleton1.ogg", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_skeleton2.ogg", 1.0f },
                    { "Audio/Characters/Damage/damage_pierce_skeleton3.ogg", 1.0f },
                },
            },
            {
                Character::MaterialType_Type_Bone,
                Character::DamageType_Source_Crushing,
                {
                },
            },
            {
                Character::MaterialType_Type_Bone,
                Character::DamageType_Source_Explosion,
                {
                    { "Audio/Characters/Damage/damage_crush_flesh1.wav", 1.0f },
                    { "Audio/Characters/Damage/damage_crush_flesh2.wav", 1.0f },
                },
            },
        };

        static const SoundPathVector StanardHealingSoundPaths = 
        {
            { "Audio/Items/UI/item_Healing_1.wav", 1.0f },
            { "Audio/Items/UI/item_Healing_2.wav", 1.0f },
            { "Audio/Items/UI/item_Healing_3.wav", 1.0f },
        };

        const std::vector<DamageSoundPaths>& GetStandardOnFleshDamageSoundPaths()
        {
            return StandardOnFleshDamageSoundPaths;
        }

        const std::vector<DamageSoundPaths>& GetStandardOnClothDamageSoundPaths()
        {
            return StandardOnClothDamageSoundPaths;
        }

        const std::vector<DamageSoundPaths>& GetStandardOnMetalDamageSoundPaths()
        {
            return StandardOnMetalDamageSoundPaths;
        }

        const std::vector<DamageSoundPaths>& GetStandardOnBoneDamageSoundPaths()
        {
            return StandardOnBoneDamageSoundPaths;
        }

        const Dwarf::Audio::SoundPathVector& GetStandardHealingSoundPaths()
        {
            return StanardHealingSoundPaths;
        }
    }

    void EnumerateDamageSoundPreloads(PreloadSet& preloads, const std::vector<Audio::DamageSoundPaths>& sounds)
    {
        for (const auto& damageSounds : sounds)
        {
            EnumerateSoundVectorPreloads(preloads, damageSounds.Sounds);
        }
    }
}
