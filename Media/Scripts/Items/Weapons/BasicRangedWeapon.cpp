#include "Items/Weapons/BasicRangedWeapon.hpp"

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::pair<float, float> RangedWeaponSoundRadius = std::make_pair(2000.0f, 4500.0f);

        BasicRangedWeapon::BasicRangedWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicWeapon(parameters, nameCode, type, skeletonPath, matsetPath)
            , _range(0.0f)
            , _fireCount(0)
            , _maxFireCount(0)
            , _animationLength(0.0f)
            , _animationHitCount(0)
            , _fireSounds()
        {
        }

        BasicRangedWeapon::~BasicRangedWeapon()
        {
        }

        bool BasicRangedWeapon::IsHittingTarget(const Character::Character* target, Vector2f& hitPt) const
        {
            // Ranged weapon doesn't inflict damage itself
            return false;
        }

        void BasicRangedWeapon::SetRange(float range)
        {
            _range = range;
        }

        void BasicRangedWeapon::AddFireSounds(const Audio::SoundPathVector& sounds)
        {
            _fireSounds.AddSounds(sounds);
        }

        Vector2f BasicRangedWeapon::ComputeTargetAttackPoint(Character::Character *target)
        {
            const Rectanglef& targetBounds = target->GetBounds();
            float heightPerc = Random::RandomBetween(0.25f, 0.75f);
            return Vector2f(targetBounds.Middle().X, Lerp(targetBounds.Bottom(), targetBounds.Top(), heightPerc));
        }

        float BasicRangedWeapon::GetRange() const
        {
            return _range;
        }

        void BasicRangedWeapon::OnUpdate(double totalTime, float dt)
        {
            BasicWeapon::OnUpdate(totalTime, dt);

            Character::Character* owner = GetOwner();
            if (owner && IsAttacking() && _fireCount < _maxFireCount)
            {
                const Character::Damage& dps = GetDPS();
                Character::Damage dmg = dps * (_animationLength / _animationHitCount);

                if (IsA<Character::BasicCharacter>(owner))
                {
                    if (AsA<Character::BasicCharacter>(owner)->HasRangedAttackFireAnimationPassed(this))
                    {
                        Fire(dmg);
                        _fireCount++;
                    }
                }
                else
                {
                    Fire(dmg);
                    _fireCount++;
                }
            }
        }

        void BasicRangedWeapon::OnBeginAttack(float animationLength, uint32_t hitCount)
        {
            BasicWeapon::OnBeginAttack(animationLength, hitCount);
            _fireCount = 0;
            _maxFireCount = hitCount;
            _animationLength = animationLength;
            _animationHitCount = hitCount;
        }

        void BasicRangedWeapon::OnEndAttack()
        {
            BasicWeapon::OnEndAttack();
        }

        void BasicRangedWeapon::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicWeapon::OnLoadContent(contentManager);

            _fireSounds.LoadContent(contentManager);
        }

        void BasicRangedWeapon::OnUnloadContent()
        {
            BasicWeapon::OnUnloadContent();

            _fireSounds.UnloadContent();
        }

        void BasicRangedWeapon::Fire(const Character::Damage& dmg)
        {
            auto soundManager = GetLevel()->GetSoundManager();
            auto fireSound = _fireSounds.GetNextSound();
            soundManager->PlaySinglePositionalSound(fireSound, Audio::SoundPriority::High, GetPosition(), RangedWeaponSoundRadius.first, RangedWeaponSoundRadius.second, 1.0f);
        }
    }

    template <>
    void EnumeratePreloads<Item::BasicRangedWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicWeapon>(preloads);
    }
}
