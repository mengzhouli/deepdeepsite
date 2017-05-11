#pragma once

#include "Particles/ParticleSystemInstance.hpp"

#include "Characters/SkeletonCharacter.hpp"

#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Explosive : public SkeletonCharacter
        {
        public:
            Explosive(const CharacterParameters& parameters, const std::string& skeleton, const std::string& matset,
                      const std::string& fuseName, const std::string& exposionOrigin,
                      CharacterID owner, float duration, float radius, float dmg);

            virtual void OnSpawn() override;

            void Start();
            void Stop();
            void Toggle();

            virtual void OnUpdate(double totalTime, float dt) override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

        protected:
            void SetExplosionParticles(const std::string& particlePath);

            void SetDrawSkeletonAfterExplosion(bool drawSkeleton);
            void SetHitsEnemiesOnly(bool hitsEnemiesOnly, const std::vector<CharacterID>& exceptions);

            void AddFuseSounds(const Audio::SoundPathVector& paths);
            void AddExplosionSounds(const Audio::SoundPathVector& paths);

            bool HasExploded() const;

        private:
            CharacterID _owner;

            bool _hitsEnemiesOnly;
            std::vector<CharacterID> _hitExceptions;

            bool _drawSkeletonAfterExplosion;

            bool _countingDown;
            float _durationRemaining;

            float _timeDead;
            float _deathFadeBegin;
            float _deathFadeDuration;

            float _blastRadius;
            float _blastImpulse;
            float _dmg;

            std::string _fuseName;
            std::string _explosionOrigin;

            std::string _fuseParticlesPath;
            ResourcePointer< Particles::ParticleSystemInstance> _fuse;

            std::string _explosionParticlesPath;
            ResourcePointer<Particles::ParticleSystemInstance> _explosion;

            float _blastLightDuration;
            Lights::PointLight _explosionLight;
            Lights::PointLight _fuseLight;

            bool _exploding;

            Audio::SoundSet _fuseSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _currentFuseSound;

            Audio::SoundSet _explosionSounds;
        };
    }

    template <>
    void EnumeratePreloads<Character::Explosive>(PreloadSet& preloads);
}
