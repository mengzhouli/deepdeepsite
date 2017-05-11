#pragma once

#include "Items/Weapons/BasicRangedWeapon.hpp"
#include "Characters/SkeletonCharacter.hpp"

namespace Dwarf
{
    namespace Item
    {
        class ThrowingWeapon : public BasicRangedWeapon
        {
        public:
            ThrowingWeapon(const ItemParameters& parameters, const std::string& nameCode, const std::string& skeletonPath,
                           const std::vector<std::string>& materialPaths);

        protected:
            void SetSpinLimits(const Rotatorf& minRotPerSec, const Rotatorf& maxRotPerSec);

            virtual void SetMaterialCollisionSound(const Audio::SoundPathVector& sounds) override;

            virtual void Fire(const Character::Damage& dmg) override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

        private:
            const float _fadeDelay;
            const float _fadeDuration;
            float _fadeTimer;

            Rotatorf _minSpin;
            Rotatorf _maxSpin;

            const std::string _skeletonPath;
            const std::vector<std::string> _materialPaths;

            uint32_t _curSkeletonIndex;
            std::vector<Animation::SkeletonInstance*> _skeletons;

            Audio::SoundPathVector _bounceSounds;
        };
    }

    template <>
    void EnumeratePreloads<Item::ThrowingWeapon>(PreloadSet& preloads);
}
