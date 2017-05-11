#include "Characters/Dummy.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string DummySkeletonPath = "Skeletons/Characters/Dummy/hugnlumps.skel";
        static const std::string DummyMatsetPath = "Skeletons/Characters/Dummy/hugnlumps.polymatset";
        static const std::string DummyDamageParticlesPath = "Particles/straw.partsys";

        Dummy::Dummy(const CharacterParameters& parameters)
            : BasicCharacter(parameters, DummySkeletonPath, DummyMatsetPath)
        {
            SetMoveType(MoveType_None);
            SetAttachToGroundOnSpawn(true);
            SetMaxHealth(10000.0f);
            SetHealthRegeneration(10000.0f);

            SetHeadName("head");

            SetSkeletonCastsShadows(false);

            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            AddDamagedParticleSystem(DamageType_Element_Physical, DummyDamageParticlesPath, "");

            //AddIdleAnimation("idle1", 1.0f);
            _damagedAnimations.AddAnimation("damage", 1.0f);

            SetBodyMaterial(MaterialType_Type_Flesh);
            AddDamageSounds(Audio::GetStandardOnFleshDamageSoundPaths());
        }

        void Dummy::OnPostRecieveDamage(Character* attacker, const Vector2f& position, const ResolvedDamage& dmg)
        {
            BasicCharacter::OnPostRecieveDamage(attacker, position, dmg);

            if (dmg.Amount > 0.0f)
            {
                PlayAnimationSet(_damagedAnimations, false, 0.1f, 0.0f);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::Dummy>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert(Character::DummySkeletonPath);
        preloads.insert(Character::DummyMatsetPath);
        preloads.insert(Character::DummyDamageParticlesPath);

        EnumerateDamageSoundPreloads(preloads, Audio::GetStandardOnFleshDamageSoundPaths());
    }
}
