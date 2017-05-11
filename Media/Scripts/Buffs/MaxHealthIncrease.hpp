#pragma once

#include "Buffs/BasicBuff.hpp"

namespace Dwarf
{
    namespace Character
    {
        class MaxHealthIncrease : public BuffComponent
        {
        public:
            MaxHealthIncrease(const BuffComponentParameters& parameters, CharacterID healer, float ammount);

        protected:
            void OnApply() override;
            void OnFinish() override;
            void OnUpdate(double totalTime, float dt) override;

        private:
            const CharacterID _healer;
            const float _ammount;

            float _scaleIncrease;
            float _originalScale;

            float _timer;
        };
    }

    template <>
    void EnumeratePreloads<Character::MaxHealthIncrease>(PreloadSet& preloads);
}
