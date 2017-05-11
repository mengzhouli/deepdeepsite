#include "Buffs/MaxHealthIncrease.hpp"
#include "Character/Character.hpp"

namespace Dwarf
{
    namespace Character
    {
        MaxHealthIncrease::MaxHealthIncrease(const BuffComponentParameters& parameters, CharacterID healer, float ammount)
            : BuffComponent(parameters)
            , _healer(healer)
            , _ammount(ammount)
            , _scaleIncrease(0.0f)
            , _originalScale(1.0f)
            , _timer(0.0f)
        {
        }

        void MaxHealthIncrease::OnApply()
        {
            Character* owner = GetOwner();
            _originalScale = owner->GetScale();
            _scaleIncrease = _ammount / owner->GetHealth().GetMaximum();
            owner->SetMaxHealth(owner->GetHealth().GetMaximum() + _ammount);
        }

        void MaxHealthIncrease::OnFinish()
        {
            Character* owner = GetOwner();
            owner->SetMaxHealth(owner->GetHealth().GetMaximum() - _ammount);
            owner->SetScale(_originalScale);
        }

        void MaxHealthIncrease::OnUpdate(double totalTime, float dt)
        {
            _timer += dt;

            const float scaleIncreaseTime = 0.4f;
            float timeSinceStart = _timer;

            float scale = 1.0f;
            if (timeSinceStart <= scaleIncreaseTime)
            {
                scale = timeSinceStart / scaleIncreaseTime;
            }

            Character* owner = GetOwner();
            owner->SetScale(_originalScale + (_scaleIncrease * scale));

            BuffComponent::OnUpdate(totalTime, dt);
        }
    }

    template <>
    void EnumeratePreloads<Character::MaxHealthIncrease>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
    }
}
