#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class Flame;
    }

    namespace Character
    {
        typedef std::function<void(Character*)> OnCheckpointUseCallback;

        class Checkpoint : public SkeletonCharacter
        {
        public:
            Checkpoint(const CharacterParameters& parameters, const Color& flameColor, OnCheckpointUseCallback callback, bool initialState, bool captureOnFirstUpdate);

        protected:
            virtual float OnPreInteractedWith(Character* interactor) override;
            virtual void OnPostInteractedWith(Character* interactor) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnSpawn() override;
            virtual void OnDeath() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            const Vector2f& getAudioPosition() const;
            std::pair<float, float> getAudioRadius() const;

            bool _initialState;
            bool _state;
            bool _captureOnFirstUpdate;
            OnCheckpointUseCallback _callback;

            Audio::SoundSet _openSounds;
            Audio::SoundSet _idleSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curIdleSound;

            std::vector<std::pair<std::string, std::shared_ptr<Graphics::Flame>>> _flames;
            std::vector<std::pair<std::string, Animation::SkeletonInstance*>> _attachedSkeletons;
        };
    }

    template <>
    void EnumeratePreloads<Character::Checkpoint>(PreloadSet& preloads);
}
