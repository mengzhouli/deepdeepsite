#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "SoundSet.hpp"

#include <functional>

namespace Dwarf
{
    namespace Character
    {
        typedef std::function<void(Character*, bool)> OnLeverUseCallback;

        class Lever : public SkeletonCharacter
        {
        public:
            Lever(const CharacterParameters& parameters, OnLeverUseCallback callback, bool initialState);

        protected:
            float OnPreInteractedWith(Character* interactor) override;
            void OnPostInteractedWith(Character* interactor) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

        protected:
            void OnSpawn() override;

            void OnUpdate(double totalTime, float dt) override;

        private:
            CharacterID _interactor = 0;
            float _interactionTimer = -1.0f;

            bool _state;
            OnLeverUseCallback _callback;
            Audio::SoundSet _moveSounds;
        };
    }

    template <>
    void EnumeratePreloads<Character::Lever>(PreloadSet& preloads);
}
