#pragma once

#include "Buffs/BasicBuff.hpp"

namespace Dwarf
{
    namespace Character
    {
        class HealOverTime : public BuffComponent
        {
        public:
            HealOverTime(const BuffComponentParameters& parameters, CharacterID source, float healthPerSecond, float tickRate);

        protected:
            virtual void OnUpdate(double totalTime, float dt) override;

        private:
            const CharacterID _source;
            const float _tickRate;
            const float _healthPerSecond;

            float _timer;
        };
    }

    template <>
    void EnumeratePreloads<Character::HealOverTime>(PreloadSet& preloads);
}
