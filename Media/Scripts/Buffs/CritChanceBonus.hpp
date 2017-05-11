#pragma once

#include "Buffs/BasicBuff.hpp"

namespace Dwarf
{
    namespace Character
    {
        class CritChanceBonus : public BuffComponent
        {
        public:
            CritChanceBonus(const BuffComponentParameters& parameters, float critChance, float critDmg);

        protected:
            void OnUpdate(double totalTime, float dt) override;

        private:
            const float _chance;
            const float _dmg;
        };
    }

    template <>
    void EnumeratePreloads<Character::CritChanceBonus>(PreloadSet& preloads);
}
