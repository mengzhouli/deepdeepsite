#include "Buffs/HealOverTime.hpp"
#include "Character/Character.hpp"
#include "Level/LevelLayerInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        HealOverTime::HealOverTime(const BuffComponentParameters& parameters, CharacterID source, float healthPerSecond, float tickRate)
            : BuffComponent(parameters)
            , _source(source)
            , _healthPerSecond(healthPerSecond)
            , _tickRate(tickRate)
            , _timer(tickRate)
        {
        }

        void HealOverTime::OnUpdate(double totalTime, float dt)
        {
            BuffComponent::OnUpdate(totalTime, dt);

            _timer -= dt;
            while (_timer <= 0.0f)
            {
                Character* owner = GetOwner();
                Character* healer = owner->GetLevelLayer()->GetCharacter(_source);
                owner->Heal(healer, _healthPerSecond * _tickRate);

                _timer += _tickRate;
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::HealOverTime>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
    }
}
