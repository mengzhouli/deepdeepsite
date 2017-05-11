#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Arrow : public SkeletonCharacter
        {
        public:
            Arrow(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath, const std::string& hitMatsetPath,
                  CharacterID owner, const Damage& dmg);

        protected:
            virtual ~Arrow();

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual Physics::Collision* CreateCollision() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override;

            virtual void OnSpawn() override;
            virtual void OnDeath() override;

        private:
            void releaseHitSkeleton();

            CharacterID _owner;
            Damage _dmg;

            Audio::SoundSet _hitGroundSounds;

            std::string _hitmatsetPath;
            const Graphics::PolygonMaterialSet* _hitMatset;

            bool _hitSomething = false;
            CharacterID _hitCharacter = 0;
            Animation::SkeletonInstance* _hitSkeleton = nullptr;
            uint32_t _hitJointIdx = 0;
            Rotatorf _rotOffset;
            Animation::Transformation _hitTransform;

            const float _totalFadeTime;
            const float _totalTime;
            float _timer;

            float _collisionRadius;

            Vector2f _prevPos;
        };
    }

    template <>
    void EnumeratePreloads<Character::Arrow>(PreloadSet& preloads);
}
