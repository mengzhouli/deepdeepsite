#pragma once

#include "Buffs/BasicBuff.hpp"

namespace Dwarf
{
    namespace Character
    {
        class SpeedBoost : public BuffComponent
        {
        public:
            SpeedBoost(const BuffComponentParameters& parameters, float percent);

        protected:
            virtual void OnUpdate(double totalTime, float dt) override;

        private:
            const float _percent;
        };
    }

    template <>
    void EnumeratePreloads<Character::SpeedBoost>(PreloadSet& preloads);
}
