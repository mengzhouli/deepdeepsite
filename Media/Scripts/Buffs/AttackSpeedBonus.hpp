#pragma once

#include "Buffs/BasicBuff.hpp"

namespace Dwarf
{
    namespace Character
    {
        class AttackSpeedBonus : public BuffComponent
        {
        public:
            AttackSpeedBonus(const BuffComponentParameters& parameters, float ammount);

        protected:
            void OnUpdate(double totalTime, float dt) override;

        private:
            const float _ammount;
        };
    }

    template <>
    void EnumeratePreloads<Character::AttackSpeedBonus>(PreloadSet& preloads);
}
