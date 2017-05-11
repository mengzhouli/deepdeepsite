#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        enum DoorSize
        {
            DoorSize_Small,
            DoorSize_Medium,
            DoorSize_Large,
            DoorSize_Huge,
        };

        typedef std::function<bool(bool)> OpenStateChangeCallback;

        class Door : public SkeletonCharacter
        {
        public:
            Door(const CharacterParameters& parameters, DoorSize size, const std::string& collisionClosedName, const std::string& collisionOpenName, bool startOpen, bool inverted, OpenStateChangeCallback callback);

            float SetOpen(bool open);

            bool IsOpen() const;

        protected:
            void OnSpawn() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            const DoorSize _size;

            const bool _inverted;

            const bool _startOpen;
            bool _state;
            std::string _collisionClosedName;
            std::string _collisionOpenName;

            Particles::ParticleSystemInstance* _hitParticles = nullptr;

            std::vector<Audio::SoundSet> _doorHitSounds;
            Audio::SoundSet _doorChainSounds;

            const OpenStateChangeCallback _callback;
        };
    }

    template <>
    void EnumeratePreloads<Character::Door>(PreloadSet& preloads);
}
