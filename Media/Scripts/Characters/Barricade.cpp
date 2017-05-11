#include "Characters/Barricade.hpp"

namespace Dwarf
{
    namespace Character
    {
        Barricade::Barricade(const CharacterParameters& parameters, float maxHealth)
            : BasicCharacter(parameters, "Skeletons/Characters/Barricade/Barricade.skel",
                                "Skeletons/Characters/Barricade/Barricade.polymatset")
        {
            SetMoveType(MoveType_None);
            SetMaxHealth(maxHealth);
            SetHealthRegeneration(0.0f);
            DisablePhysics();
            SetSkeletonJointStrength(0.0f);
            SetEntityMask(CharacterMask_Normal | CharacterMask_Constructed);

            SetSkeletonCastsShadows(false);

            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            _damagedAnimations.AddAnimation("damage", 1.0f);
        }

        void Barricade::OnPostRecieveDamage(Character* attacker, const Vector2f& position, const ResolvedDamage& dmg)
        {
            BasicCharacter::OnPostRecieveDamage(attacker, position, dmg);

            if (dmg.Amount > 0.0f)
            {
                PlayAnimationSet(_damagedAnimations, false, 0.1f, 0.0f);
            }
        }

        void Barricade::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                EnablePhysics(true);
            }

            BasicCharacter::OnStateChange(newState);
        }
    }

    template <>
    void EnumeratePreloads<Character::Barricade>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);

        preloads.insert("Skeletons/Characters/Barricade/Barricade.skel");
        preloads.insert("Skeletons/Characters/Barricade/Barricade.polymatset");
    }
}
