#include "Buffs/SpeedBoost.hpp"
#include "Character/Character.hpp"

namespace Dwarf
{
    namespace Character
    {
        SpeedBoost::SpeedBoost(const BuffComponentParameters& parameters, float percent)
            : BuffComponent(parameters)
            , _percent(percent)
        {
        }

        void SpeedBoost::OnUpdate(double totalTime, float dt)
        {
            BuffComponent::OnUpdate(totalTime, dt);

            Character* owner = GetOwner();
            owner->AddMoveSpeedMultiplier(_percent);
        }
    }

    template <>
    void EnumeratePreloads<Character::SpeedBoost>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
    }
}
