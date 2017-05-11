#include "Buffs/CritChanceBonus.hpp"
#include "Character/Character.hpp"

namespace Dwarf
{
    namespace Character
    {
        CritChanceBonus::CritChanceBonus(const BuffComponentParameters& parameters, float critChance, float critDmg)
            : BuffComponent(parameters)
            , _chance(critChance)
            , _dmg(critDmg)
        {
        }

        void CritChanceBonus::OnUpdate(double totalTime, float dt)
        {
            GetOwner()->AddCriticalChance(_chance);
            GetOwner()->AddCriticalDamage(_dmg);
        }
    }

    template <>
    void EnumeratePreloads<Character::CritChanceBonus>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BuffComponent>(preloads);
    }
}
