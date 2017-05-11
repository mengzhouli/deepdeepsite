#pragma once

#include "Character/Character.hpp"
#include "Content/Preload.hpp"
#include "SoundSet.hpp"
#include "Audio/SoundManager.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class GrappleRopeDrawable;
    }

    namespace Physics
    {
        class RopeCollision;
    }

    namespace Character
    {
        class GrappleRope : public Character
        {
        public:
            GrappleRope(const CharacterParameters& parameters, const Vector2f& anchor, const Vector2f& source, const Vector2f& dest);
            GrappleRope(const CharacterParameters& parameters, const Vector2f& anchor, const Vector2f& source, const Vector2f& dest, float travelTime);

            const Vector2f& GetAnchor() const;
            const Vector2f& GetSource() const;
            const Vector2f& GetDestination() const;

            virtual void OnSpawn() override;

            virtual void OnStateChange(CharacterState newState) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual const Rectanglef& GetBounds() const override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        protected:
            virtual ~GrappleRope();

        private:
            uint32_t _linkCount;

            float _curTime;
            float _totalTime;
            bool _finished;

            bool _createdPath;
            float _createPathTime;

            Vector2f _anchor;
            Vector2f _source;
            Vector2f _dest;

            Chainf _curRope;
            Chainf _targetRope;

            bool _anchorHit;

            Graphics::GrappleRopeDrawable* _rope;
            Physics::RopeCollision* _collision;

            Pathfinding::PathItemID _pathId;

            Audio::SoundSet _grappleHitSounds;
            Audio::SoundSet _grappleRopeSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _currentRopeSound;
        };
    }

    template <>
    void EnumeratePreloads<Character::GrappleRope>(PreloadSet& preloads);
}
