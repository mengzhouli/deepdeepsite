#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class MinerDwarf : public Dwarf, public SackSmasher
        {
        public:
            MinerDwarf(const CharacterParameters& parameters);
            MinerDwarf(const CharacterParameters& parameters, const DwarfInfo& info);

        protected:
            float OnPreInteractWithCharacter(Character* target) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            bool IsMining() const;

            SackSmasher::Info PlaySackSmashAnimation() override;

        private:
            Item::Resources _nextMineAmmount;

            CharacterID _miningTarget;

            std::string _miningAnimationHitTag;
            Animation::AnimationSet _miningAnimations;

            Particles::ParticleSystemInstance* _goldHitParticleSystem;

            const Graphics::Font* _goldFont;

            bool _usingSprintAnimations = false;
        };
    }

    template <>
    void EnumeratePreloads<Character::MinerDwarf>(PreloadSet& preloads);
}
