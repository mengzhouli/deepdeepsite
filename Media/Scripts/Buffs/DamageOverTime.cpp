#include "Buffs/DamageOverTime.hpp"
#include "Character/Character.hpp"
#include "Level/LevelLayerInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        DamageOverTime::DamageOverTime(const BuffComponentParameters& parameters, CharacterID source, const Damage& dps, float tickRate)
            : BuffComponent(parameters)
            , _source(source)
            , _dps(dps)
            , _tickRate(tickRate)
            , _timer(tickRate)
        {
        }

        void DamageOverTime::OnFinish()
        {
            BuffComponent::OnFinish();

            _finished = true;
        }

        void DamageOverTime::OnUpdate(double totalTime, float dt)
        {
            BuffComponent::OnUpdate(totalTime, dt);

            if (!_finished)
            {
                _timer -= dt;
                while (_timer <= 0.0f)
                {
                    Character* owner = GetOwner();
                    Character* inflicter = owner->GetLevelLayer()->GetCharacter(_source);
                    owner->ApplyDamage(inflicter, owner->GetBounds().Middle(), _dps * _tickRate);

                    _timer += _tickRate;
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::DamageOverTime>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
    }
}
