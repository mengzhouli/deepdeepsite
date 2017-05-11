#include "Buffs/AttackSpeedBonus.hpp"
#include "Character/Character.hpp"

namespace Dwarf
{
    namespace Character
    {
        AttackSpeedBonus::AttackSpeedBonus(const BuffComponentParameters& parameters, float ammount)
            : BuffComponent(parameters)
            , _ammount(ammount)
        {
        }

        void AttackSpeedBonus::OnUpdate(double totalTime, float dt)
        {
            GetOwner()->AddAttackSpeedMultiplier(_ammount);
        }
    }

    template <>
    void EnumeratePreloads<Character::AttackSpeedBonus>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BuffComponent>(preloads);
    }
}
