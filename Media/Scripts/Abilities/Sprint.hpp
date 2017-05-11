#pragma once

#include "Abilities/BasicAbility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Ability
    {
        class Sprint : public BasicAbility
        {
        public:
            Sprint(const AbilityParameters& parameters);

            void ClearState() override;
            bool IsInValidState() const override;
            Character::Action GenerateAction() const override;
            void ExecuteAction(const Character::Action& action) override;
            void CancelAction() override;
            bool IsExecutingAction() const override;

            void OnUpdate(double totalTime, float dt) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Audio::SoundSet _sprintSounds;
        };
    }

    template <>
    void EnumeratePreloads<Ability::Sprint>(PreloadSet& preloads);
}
