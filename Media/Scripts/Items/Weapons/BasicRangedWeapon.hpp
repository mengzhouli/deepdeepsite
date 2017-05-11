#pragma once

#include "Items/Weapons/BasicWeapon.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BasicRangedWeapon : public BasicWeapon
        {
        public:
            BasicRangedWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath);

            virtual bool IsHittingTarget(const Character::Character* target, Vector2f& hitPt) const override;

            virtual float GetRange() const;
            void SetRange(float range);

        protected:
            virtual ~BasicRangedWeapon();

            void AddFireSounds(const Audio::SoundPathVector& sounds);

            static Vector2f ComputeTargetAttackPoint(Character::Character *target);

            virtual void Fire(const Character::Damage& dmg);

            virtual void OnBeginAttack(float animationLength, uint32_t hitCount) override;
            virtual void OnEndAttack() override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

        private:
            float _range;
            uint32_t _fireCount;
            uint32_t _maxFireCount;
            float _animationLength;
            uint32_t _animationHitCount;

            Audio::SoundSet _fireSounds;
        };
    }

    template <>
    void EnumeratePreloads<Item::BasicRangedWeapon>(PreloadSet& preloads);
}
