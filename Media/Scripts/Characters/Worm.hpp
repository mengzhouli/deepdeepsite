#pragma once

#include "Characters/BasicCharacter.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Worm : public BasicCharacter
        {
        public:
            Worm(const CharacterParameters& parameters);

            float GetAttackRange() const;

        protected:
            virtual void OnSpawn() override;
            virtual void OnDeath() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

        private:
            void emerge();
            void hide();

            bool _visible;
            bool _emerging;
            float _emergeTimer;

            float _hidePerc;
            float _hideTimer;
            bool _hiding;
            bool _doneHiding;

            Audio::SoundSet _wormEmergedSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curWormEmergedSound;
        };
    }

    template <>
    void EnumeratePreloads<Character::Worm>(PreloadSet& preloads);
}
