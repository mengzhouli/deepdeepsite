#include "Items/Weapons/OrkWeapons.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string OrkWeaponSkeletonPath = "Skeletons/Items/Weapons/OrkWeapons/ork_weapon.skel";
        static const std::string OrkbWeaponAttachPointA = "origin";
        static const std::string OrkWeaponAttachPointB = "b";

        static const float OrkMeleeWeapDPS = 4.0f;

        OrkWeapon::OrkWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicMeleeWeapon(parameters, nameCode, type, skeletonPath, matsetPath)
            , _doesKnockback(false)
        {
            SetSplash(true);
        }

        void OrkWeapon::OnPostInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& damage)
        {
            BasicMeleeWeapon::OnPostInflictDamage(target, position, damage);

            if (_doesKnockback)
            {
                Character::Character* owner = GetOwner();
                assert(owner != nullptr);

                Physics::Collision* collision = GetCollision();
                assert(collision != nullptr);

                std::vector<Vector2f> hitPts;
                std::vector<Character::Character*> hitTargets =
                    GetLevelLayer()->FindIntersections<Character::Character>(collision, hitPts, [&](const Character::Character* target)
                {
                    return owner->IsCharacterAttackable(target, false);
                });

                for (auto& hitTarget : hitTargets)
                {
                    Physics::Collision* hitTargetCollision = hitTarget->GetCollision();
                    if (hitTargetCollision == nullptr)
                    {
                        continue;
                    }

                    Vector2f dir = Vector2f::Normalize(hitTargetCollision->GetBounds().Middle() - owner->GetBounds().Middle());
                    float impulse = hitTargetCollision->GetMass() * 0.001f;
                    hitTarget->ApplyLinearImpulse(dir * impulse);
                }
            }
        }

        static const std::string OrkChopperMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/chopper.polymatset";

        OrkChopper::OrkChopper(const ItemParameters& parameters)
            : OrkWeapon(parameters, "", WeaponType_Melee_1H, OrkWeaponSkeletonPath, OrkChopperMatsetPath)
        {
            SetAttachPoints(OrkbWeaponAttachPointA, OrkWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, OrkMeleeWeapDPS));
        }

        static const std::string OrkClubMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/club.polymatset";

        OrkClub::OrkClub(const ItemParameters& parameters)
            : OrkWeapon(parameters, "", WeaponType_Melee_1H, OrkWeaponSkeletonPath, OrkClubMatsetPath)
        {
            SetAttachPoints(OrkbWeaponAttachPointA, OrkWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, OrkMeleeWeapDPS));
        }

        static const std::string OrkHookMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/hook.polymatset";

        OrkHook::OrkHook(const ItemParameters& parameters)
            : OrkWeapon(parameters, "", WeaponType_Melee_1H, OrkWeaponSkeletonPath, OrkHookMatsetPath)
        {
            SetAttachPoints(OrkbWeaponAttachPointA, OrkWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Slashing | Character::DamageType_Element_Physical, OrkMeleeWeapDPS));
        }

        static const std::string OrkMaceMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/mace.polymatset";

        OrkMace::OrkMace(const ItemParameters& parameters)
            : OrkWeapon(parameters, "", WeaponType_Melee_1H, OrkWeaponSkeletonPath, OrkMaceMatsetPath)
        {
            SetAttachPoints(OrkbWeaponAttachPointA, OrkWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, OrkMeleeWeapDPS));
        }

        static const std::string OrkLanceSkeletonPath = "Skeletons/Items/Weapons/OrkWeapons/lance.skel";
        static const std::string OrkLanceMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/lance.polymatset";
        static const float OrkLanceDPS = 5.0f;

        OrkLance::OrkLance(const ItemParameters& parameters)
            : OrkWeapon(parameters, "", WeaponType_Melee_1H, OrkLanceSkeletonPath, OrkLanceMatsetPath)
        {
            SetAttachPoints(OrkbWeaponAttachPointA, OrkWeaponAttachPointB);
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, OrkLanceDPS));
        }

        static const std::string OrkTowerShieldSkelPath = "Skeletons/Items/Weapons/OrkWeapons/tower_shield.skel";
        static const std::string OrkTowerShieldMatsetPath = "Skeletons/Items/Weapons/OrkWeapons/tower_shield.polymatset";
        static const float OrkShieldDPS = 3.0f;

        static const std::vector<std::string> OrkTowerShieldBreakablePieces =
        {
            "break_0",
            "break_1",
            "break_2",
            "break_3",
        };

        static const Audio::SoundPathVector OrkTowerShieldCollisionSounds =
        {
            { "Audio/Characters/Debris/Debris_bouncing.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_3.wav", 1.0f },
        };

        OrkTowerShield::OrkTowerShield(const ItemParameters& parameters)
            : BasicWeapon(parameters, "", WeaponType_Melee_Shield, OrkTowerShieldSkelPath, OrkTowerShieldMatsetPath)
            , _curBreakPiece(0)
        {
            SetAttachPoints("a", "b");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, OrkShieldDPS));
            SetMaterialCollisionSound(OrkTowerShieldCollisionSounds);
        }

        bool OrkTowerShield::DoesSplashDamage() const
        {
            return true;
        }

        uint32_t OrkTowerShield::GetBreakablePieceCount() const
        {
            return OrkTowerShieldBreakablePieces.size();
        }

        void OrkTowerShield::BreakNextPiece()
        {
            if (_curBreakPiece < OrkTowerShieldBreakablePieces.size())
            {
                BreakMaterial(OrkTowerShieldBreakablePieces[_curBreakPiece]);
                _curBreakPiece++;
            }
        }
    }

    template <>
    void EnumeratePreloads<Item::OrkWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);
    }

    template <>
    void EnumeratePreloads<Item::OrkChopper>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::OrkWeapon>(preloads);

        preloads.insert(Item::OrkWeaponSkeletonPath);
        preloads.insert(Item::OrkChopperMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::OrkClub>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::OrkWeapon>(preloads);

        preloads.insert(Item::OrkWeaponSkeletonPath);
        preloads.insert(Item::OrkClubMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::OrkHook>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::OrkWeapon>(preloads);

        preloads.insert(Item::OrkWeaponSkeletonPath);
        preloads.insert(Item::OrkHookMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::OrkMace>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::OrkWeapon>(preloads);

        preloads.insert(Item::OrkWeaponSkeletonPath);
        preloads.insert(Item::OrkMaceMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::OrkLance>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::OrkWeapon>(preloads);

        preloads.insert(Item::OrkLanceSkeletonPath);
        preloads.insert(Item::OrkLanceMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::OrkTowerShield>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert(Item::OrkTowerShieldSkelPath);
        preloads.insert(Item::OrkTowerShieldMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::OrkTowerShieldCollisionSounds);
    }
}
