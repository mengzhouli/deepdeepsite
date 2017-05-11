#include "Abilities/Sprint.hpp"

#include "Buffs/BasicBuff.hpp"
#include "Buffs/SpeedBoost.hpp"

namespace Dwarf
{
    static const std::string AbilityIconMatsetPath = "HUD/abilities.hudmatset";

    namespace Character
    {
        class SprintBuff : public BasicBuff
        {
        public:
            SprintBuff(const BuffParameters& parameters, float speed, float duration)
                : BasicBuff(parameters, "buff_sprint_name")
            {
                AddComponent<SpeedBoost>(speed);
                SetTotalDuration(duration);

                SetIcon(AbilityIconMatsetPath, "icon_buff_sprint");
                SetTooltipDescription("buff_sprint_description");
            }
        };
    }

    template <>
    void EnumeratePreloads<Character::SprintBuff>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicBuff>(preloads);
        EnumeratePreloads<Character::SpeedBoost>(preloads);
        preloads.insert(AbilityIconMatsetPath);
    }

    namespace Ability
    {
        static const Audio::SoundPathVector SprintSoundPaths =
        {
            { "Audio/Abilities/Navigator/Grapple_Rope.ogg", 1.0f },
        };
        static const std::pair<float, float> SprintSoundRadius = std::make_pair(1000.0f, 3500.0f);
        static const float SprintSoundVolume = 1.5f;

        static const float SprintDuration = 5.0f;
        static const float SprintSpeedBoost = 1.5f;
        static const float SprintCooldown = 15.0f;

        Sprint::Sprint(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_sprint_name", AbilityType_NoTarget)
        {
            SetIcon(AbilityIconMatsetPath, "icon_sprint");
            SetCooldown(SprintCooldown);
            SetInstant(true);
            SetTooltipDescription("ability_sprint_tooltip", (SprintSpeedBoost - 1.0f) * 100.0f, SprintDuration);

            _sprintSounds.AddSounds(SprintSoundPaths);
        }

        void Sprint::ClearState()
        {
        }

        bool Sprint::IsInValidState() const
        {
            return true;
        }

        Character::Action Sprint::GenerateAction() const
        {
            return Character::CreateUseAbilityAction(GetID());
        }
        
        static float GetSprintDuration(Character::Character* owner)
        {
            return SprintDuration;
        }

        static float GetSprintSpeedBoost(Character::Character* owner)
        {
            return SprintSpeedBoost;
        }

        void Sprint::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();

            float duration = GetSprintDuration(owner);
            float boost = GetSprintSpeedBoost(owner);
            owner->ApplyBuff(Character::BindBuffConstructor<Character::SprintBuff>(boost, duration));

            auto soundManager = owner->GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_sprintSounds.GetNextSound(), Audio::SoundPriority::High, owner->GetPosition(), SprintSoundRadius.first, SprintSoundRadius.second, SprintSoundVolume);

            StartCooldown();
        }

        void Sprint::CancelAction()
        {
        }
        
        bool Sprint::IsExecutingAction() const
        {
            return false;
        }

        void Sprint::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);
        }

        void Sprint::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _sprintSounds.LoadContent(contentManager);
        }

        void Sprint::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            _sprintSounds.UnloadContent();
        }

        void Sprint::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);
        }
    }

    template <>
    void EnumeratePreloads<Ability::Sprint>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);
        EnumeratePreloads<Character::SprintBuff>(preloads);

        preloads.insert(AbilityIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Ability::SprintSoundPaths);
    }
}
