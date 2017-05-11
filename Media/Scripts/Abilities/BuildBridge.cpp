#include "Abilities/BuildBridge.hpp"

#include "Item/Weapon.hpp"

#include "Characters/CharacterTraits.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

#include "AbilityUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string BuildBridgeIconPath = "HUD/abilities.hudmatset";
        static const std::string BuildBridgeCursorPath = "HUD/abilities.cursorset";

        BuildBridge::BuildBridge(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_build_bridge_name", AbilityType_TwoTargets)

            , _minLength(500.0f)
            , _maxLength(10000.0f)
            , _bridgeBuildTime(10.0f)

            , _startPos(Vector2f::Zero)
            , _startMovePos(Vector2f::Zero)
            , _endPos(Vector2f::Zero)

            , _startValid(false)
            , _endValid(false)
            , _bridgeValid(false)

            , _bridge(NULL)

            , _drawBridge(false)
            , _bridgeDrawable(new Graphics::BridgeDrawable())

            , _executing(false)
        {
            SetIcon(BuildBridgeIconPath, "icon_build_bridge");
            SetCursor(BuildBridgeCursorPath, "cursor_build");
            SetCooldown(5.0f);
            SetResourceCost(Item::Resources(25));
            SetTooltipDescription("ability_build_bridge_tooltip", _maxLength);
        }

        BuildBridge::~BuildBridge()
        {
            SafeRelease(_bridgeDrawable);
        }

        void BuildBridge::ClearState()
        {
            _startPos = Vector2f::Zero;
            _endPos = Vector2f::Zero;
            _startValid = false;
            _endValid = false;
            _bridgeValid = false;
            _executing = false;
        }

        bool BuildBridge::IsExecutingAction() const
        {
            return _executing;
        }

        void BuildBridge::CancelAction()
        {
            if (_executing)
            {
                Character::Character* owner = GetOwner();
                owner->StopMoving();
                if (_bridge)
                {
                    _bridge->Terminate();
                    _bridge = nullptr;
                }
                _executing = false;
                _drawBridge = false;
                _startValid = false;
                _endValid = false;
                _drawBridge = false;
            }
        }

        void BuildBridge::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();

            Vector2f start(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            Vector2f end(action.AbilityPosition.EndX, action.AbilityPosition.EndY);

            _bridgeDrawable->SetEndpoints(start, end);

            if (VerifyBridge(start, end, _bridgeDrawable->GetPathChain()))
            {
                // Todo, calculate moveable position from the action
                _executing = owner->MoveTo(_startMovePos);
                _startPos = start;
                _endPos = end;

                float buildTime = _bridgeBuildTime;

                for (Item::BuildRateModifier *buildModifierWeap : owner->GetWeapons<Item::BuildRateModifier>())
                {
                    buildTime /= buildModifierWeap->GetBuildRateMultiplier();
                }

                Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                _bridge = layer->SpawnCharacter(_startPos, "bridge", owner->GetController(),
                                                Character::BindCharacterConstructor<Character::Bridge>(_startPos, _endPos, buildTime));
            }

            _drawBridge = false;
        }

        Character::Action BuildBridge::GenerateAction() const
        {
            if (_bridgeValid)
            {
                return Character::CreateUseAbilityAction(GetID(), _startPos, _endPos);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        bool BuildBridge::IsInValidState() const
        {
            return _bridgeValid;
        }

        void BuildBridge::SetSecondaryTarget(const Vector2f& pos)
        {
            bool startAndEndValid = _startValid && VerifyBridgeEnd(pos, _endPos);
            _bridgeDrawable->SetEndpoints(_startPos, _endPos);
            bool allValid = startAndEndValid && VerifyBridge(_startPos, _endPos, _bridgeDrawable->GetPathChain());
            if (allValid)
            {
                _endValid = true;
                _bridgeValid = true;
            }
            else
            {
                _endValid = false;
                _bridgeValid = false;
                _endPos = pos;
            }
            _drawBridge = true;
        }

        void BuildBridge::SetPrimaryTarget(const Vector2f& pos)
        {
            _bridgeValid = false;
            Character::Character* owner = GetOwner();
            if (VerifyBridgeStart(pos, _startPos, _startMovePos) && owner->CanMoveTo(_startMovePos))
            {
                _startValid = true;
                _endValid = false;
                _endPos = pos;
                _drawBridge = true;
            }
            else
            {
                _startValid = false;
                _endValid = false;
                _drawBridge = false;
            }
            _endPos = pos;
        }

        void BuildBridge::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _bridgeDrawable->LoadContent(contentManager);
        }

        void BuildBridge::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            _bridgeDrawable->UnloadContent();
        }

        void BuildBridge::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            if (_executing)
            {
                Character::Character* owner = GetOwner();

                const float minBuildDist = owner->GetBounds().W * 0.5f;
                bool closeEnoughToBuildPos = Vector2f::Distance(owner->GetPosition(), _startMovePos) < minBuildDist;

                if (!owner->IsMoving() || closeEnoughToBuildPos)
                {
                    owner->StopMoving();

                    bool isBuilding = _bridge->IsBuilding();
                    bool isComplete = _bridge->IsBuilt();
                    if (!isComplete && !isBuilding && closeEnoughToBuildPos)
                    {
                        _bridge->SetBuilding(true);

                        if (IsA<Character::Builder>(owner))
                        {
                            AsA<Character::Builder>(owner)->PlayBuildAnimation();
                        }

                        StartCooldown();
                    }
                    else if (isComplete)
                    {
                        _executing = false;
                    }
                }
            }

            if (_drawBridge)
            {
                _bridgeDrawable->SetEndpoints(_startPos, _endPos);

                _bridgeDrawable->SetColor(GetAbilityUIColor(_bridgeValid));
                _bridgeDrawable->Update(totalTime, dt);
            }
        }

        void BuildBridge::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            if (_drawBridge)
            {
                levelRenderer->AddDrawable(_bridgeDrawable, false, false);
            }
        }

        bool BuildBridge::VerifyBridge(const Vector2f& start, const Vector2f& end, const Chainf& bridge) const
        {
            float lenSq = Vector2f::DistanceSquared(start, end);
            bool lenValid = (Abs(Vector2f::Normalize(start - end).Y) < 0.4f &&
                lenSq <= (_maxLength * _maxLength) &&
                lenSq >= (_minLength * _minLength));
            if (!lenValid)
            {
                return false;
            }

            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();

            if (bridge.Size() < 3)
            {
                return false;
            }

            // Build a spline out of the bridge
            Splinef spline(bridge.Points());

            static const uint32_t hitTests = 32;
            for (uint32_t i = 0; i < hitTests; i++)
            {
                float t = float(i + 1) / (hitTests + 1);
                Vector2f pos = spline.Evalulate(t);

                Pathfinding::EdgeType hitEdgeType;
                if (layer->HitTerrain(pos, hitEdgeType, Pathfinding::EdgeType_Walk))
                {
                    return false;
                }
            }

            return true;
        }

        bool BuildBridge::VerifyBridgeEnd(const Vector2f& pos, Vector2f& outPos) const
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();

            std::shared_ptr<Pathfinding::PathPosition> ledgePos = layer->FindLedge(pos, _ledgeSearchRadius, Pathfinding::EdgeType_Walk);
            if (ledgePos != nullptr)
            {
                assert(ledgePos->GetType() == Pathfinding::EdgeType_Walk);
                outPos = ledgePos->GetPosition();
                return true;
            }
            else
            {
                outPos = pos;
                return false;
            }
        }

        bool BuildBridge::VerifyBridgeStart(const Vector2f& pos, Vector2f& outPos, Vector2f& outMovePos) const
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();

            std::shared_ptr<Pathfinding::PathPosition> ledgePos = layer->FindLedge(pos, _ledgeSearchRadius, Pathfinding::EdgeType_Walk);
            if (ledgePos != nullptr)
            {
                assert(ledgePos->GetType() == Pathfinding::EdgeType_Walk);
                outPos = ledgePos->GetPosition();
                outMovePos = outPos;
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::BuildBridge>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert(Ability::BuildBridgeIconPath);
        preloads.insert(Ability::BuildBridgeCursorPath);
        EnumeratePreloads<Character::Bridge>(preloads);
        EnumeratePreloads<Graphics::BridgeDrawable>(preloads);
    }
}
