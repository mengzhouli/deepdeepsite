#pragma once

#include "Buffs/DamageOverTime.hpp"
#include "Drawables/Flame.hpp"

namespace Dwarf
{
    namespace Character
    {
        class BurningDamageOverTime : public DamageOverTime
        {
        public:
            BurningDamageOverTime(const BuffComponentParameters& parameters, CharacterID source, float dps, float tickRate);

        protected:
            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnApply() override;
            void OnFinish() override;
            bool CanTerminate() const override;

            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            void updateFlamePositions();

            Graphics::Flame _flame;
        };
    }

    template <>
    void EnumeratePreloads<Character::BurningDamageOverTime>(PreloadSet& preloads);
}