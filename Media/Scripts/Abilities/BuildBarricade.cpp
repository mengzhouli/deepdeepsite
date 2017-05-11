#include "Abilities/BuildBarricade.hpp"

#include "Characters/Barricade.hpp"

#include "AbilityUtility.hpp"
#include "NavigationUtility.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string BuildBarricadeIconPath = "HUD/abilities.hudmatset";
        static const std::string BuildBarricadeCursorPath = "HUD/abilities.cursorset";

        static const std::string BuildBarricadeBarricadeSkeletonPath = "Skeletons/Characters/Barricade/Barricade.skel";
        static const std::string BuildBarricadeBarricadeMatsetPath = "Skeletons/Characters/Barricade/Barricade.polymatset";

        BuildBarricade::BuildBarricade(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_build_barricade_name", AbilityType_OneTarget)
            , _executing(false)

            , _validPosition(false)
            , _drawBarricade(false)
            , _barricadePosition(Vector2f::Zero)

            , _barricadeHealth(100.0f)

            , _barricade(nullptr)
        {
            SetIcon(BuildBarricadeIconPath, "icon_build_barricade");
            SetCursor(BuildBarricadeCursorPath, "cursor_build");
            SetCooldown(30.0f);
            SetResourceCost(Item::Resources(10));
            SetTooltipDescription("ability_build_barricade_tooltip", _barricadeHealth);
        }

        void BuildBarricade::ClearState()
        {
            _validPosition = false;
            _drawBarricade = false;
        }

        void BuildBarricade::SetPrimaryTarget(const Vector2f& pos)
        {
            _validPosition = VerifyBerricadePosition(pos, _barricadePosition);
            _drawBarricade = true;
        }

        bool BuildBarricade::IsInValidState() const
        {
            return _validPosition;
        }

        Character::Action BuildBarricade::GenerateAction() const
        {
            if (_validPosition)
            {
                return Character::CreateUseAbilityAction(GetID(), _barricadePosition);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void BuildBarricade::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _barricadePosition = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _executing = owner->MoveTo(_barricadePosition);
        }

        void BuildBarricade::CancelAction()
        {
            _executing = false;
            _drawBarricade = false;
        }

        bool BuildBarricade::IsExecutingAction() const
        {
            return _executing;
        }

        void BuildBarricade::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            _barricade->SetColor(GetAbilityUIColor(_validPosition));
            _barricade->SetPosition(_barricadePosition);

            if (_executing)
            {
                Character::Character* owner = GetOwner();

                const float minDropDist = owner->GetBounds().W * 0.5f;
                bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _barricadePosition) < minDropDist;

                if (!owner->IsMoving() || closeEnoughToDropPos)
                {
                    owner->StopMoving();

                    Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                    layer->SpawnCharacter(_barricadePosition, "barricade", owner->GetController(),
                                            Character::BindCharacterConstructor<Character::Barricade>(_barricadeHealth));
                    StartCooldown();

                    _executing = false;
                    _drawBarricade = false;
                }
            }
        }

        void BuildBarricade::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _barricade = Content::CreateSkeletonInstance(contentManager, BuildBarricadeBarricadeSkeletonPath, BuildBarricadeBarricadeMatsetPath);
        }

        void BuildBarricade::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_barricade);
        }

        void BuildBarricade::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            if (_drawBarricade)
            {
                levelRenderer->AddDrawable(_barricade, false, false);
            }
        }

        bool BuildBarricade::VerifyBerricadePosition(const Vector2f& pos, Vector2f& placePos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            if (Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, placePos))
            {
                return true;
            }
            else
            {
                placePos = pos + (_barricade->GetPosition() - _barricade->GetDrawBounds().Middle());
                return false;
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::BuildBarricade>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert(Ability::BuildBarricadeIconPath);
        preloads.insert(Ability::BuildBarricadeCursorPath);
        EnumeratePreloads<Character::Barricade>(preloads);
        preloads.insert(Ability::BuildBarricadeBarricadeSkeletonPath);
        preloads.insert(Ability::BuildBarricadeBarricadeMatsetPath);
    }
}
