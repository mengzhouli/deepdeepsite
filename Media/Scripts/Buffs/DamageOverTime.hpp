#pragma once

#include "Buffs/BasicBuff.hpp"
#include "Character/Damage.hpp"

namespace Dwarf
{
    namespace Character
    {
        class DamageOverTime : public BuffComponent
        {
        public:
            DamageOverTime(const BuffComponentParameters& parameters, CharacterID source, const Damage& dps, float tickRate);

        protected:
            void OnFinish() override;
            void OnUpdate(double totalTime, float dt) override;

        private:
            const CharacterID _source;
            const float _tickRate;
            const Damage _dps;

            float _timer;

            bool _finished = false;
        };
    }

    template <>
    void EnumeratePreloads<Character::DamageOverTime>(PreloadSet& preloads);
}
