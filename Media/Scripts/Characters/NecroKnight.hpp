#pragma once

#include "Characters/BasicCharacter.hpp"

#include "Particles/ParticleSystemInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        class NecroKnight : public BasicCharacter
        {
        public:
            NecroKnight(const CharacterParameters& parameters);

        protected:
            virtual void OnUpdate(double totalTime, float dt) override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnStateChange(CharacterState newState) override;

        private:
            Particles::ParticleSystemInstance* _particles;
        };
    }

    template <>
    void EnumeratePreloads<Character::NecroKnight>(PreloadSet& preloads);
}
