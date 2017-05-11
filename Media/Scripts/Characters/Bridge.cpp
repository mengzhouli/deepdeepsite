#include "Characters/Bridge.hpp"

#include "ContentUtility.hpp"
#include "Random.hpp"
#include "Drawables/RopeUtility.hpp"
#include "SkeletonUtility.hpp"
#include "Item/Item.hpp"
#include "HUD/Tooltip.hpp"

namespace Dwarf
{
    namespace Character
    {
        Bridge::Bridge(const CharacterParameters& parameters, const Vector2f& a, const Vector2f& b, float buildTime)
            : Character(parameters)
            , _totalBuildTime(buildTime)
            , _remainingBuildTime(buildTime)
            , _buildFinished(false)
            , _building(false)
            , _a(a)
            , _b(b)
            , _bridge(nullptr)
            , _pathId(Pathfinding::PathItemID_Invalid)
        {
            _bridge = new Graphics::BridgeDrawable();
            _bridge->SetEndpoints(a, b);

            SetEntityMask(CharacterMask_Normal | CharacterMask_Constructed);
            DisablePhysics();
        }

        Bridge::~Bridge()
        {
            SafeRelease(_bridge);
        }

        bool Bridge::IsBuilt() const
        {
            return _buildFinished;
        }

        bool Bridge::IsBuilding() const
        {
            return _building;
        }

        void Bridge::SetBuilding(bool building)
        {
            _building = building;
        }

        const Vector2f& Bridge::GetSideA() const
        {
            return _a;
        }

        const Vector2f& Bridge::GetSideB() const
        {
            return _b;
        }

        void Bridge::OnLoadContent(Content::ContentManager* contentManager)
        {
            _bridge->LoadContent(contentManager);
        }

        void Bridge::OnUnloadContent()
        {
            _bridge->UnloadContent();
        }

        void Bridge::OnSpawn()
        {
            updateBuilding(0.0f);
        }

        void Bridge::OnUpdate(double totalTime, float dt)
        {
            updateBuilding(dt);
            _bridge->Update(totalTime, dt);
        }

        void Bridge::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            levelRenderer->AddDrawable(_bridge, false);
            levelRenderer->AddMinimapDrawable(_bridge);
        }

        void Bridge::updateBuilding(float dt)
        {
            if (!_buildFinished)
            {
                if (_building)
                {
                    _remainingBuildTime -= dt;
                }

                float buildPerc = (_totalBuildTime > 0.0f) ? Clamp(1.0f - (_remainingBuildTime / _totalBuildTime), 0.0f, 1.0f) : 1.0f;
                _bridge->SetBuildPercent(buildPerc);

                if (_remainingBuildTime <= 0.0f)
                {
                    Pathfinding::PathSystem* pathSystem = GetLevelLayer()->GetPathSystem();
                    _pathId = pathSystem->AddChain(_bridge->GetPathChain(), Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::Wood);

                    _buildFinished = true;
                }
            }
        }

        static const std::string BridgeEndSkeletonPath = "Skeletons/Characters/Bridge/bridge_end.skel";
        static const std::string BridgeEndMatsetPath = "Skeletons/Characters/Bridge/bridge_end.polymatset";
        static const std::string BridgeLinkSkeletonPath = "Skeletons/Characters/Bridge/bridge_link2.skel";
        static const std::string BridgeLinkMatsetPath = "Skeletons/Characters/Bridge/bridge_link2.polymatset";

        static const std::string BuildCursorPath = "HUD/abilities.cursorset";

        static const uint32_t RopeLinkMultiplier = 1;

        static const float TotalBuildTime = 8.0f;

        Bridge2::Bridge2(const CharacterParameters& parameters, const Vector2f& a, const Vector2f& b, float buildPerc, const Item::Resources& cost)
            : Character(parameters)
            , _totalCost(cost)
            , _remainingCost(cost * (1.0f - buildPerc))
            , _buildPerc(buildPerc)
        {
            _a.position = a;
            _b.position = b;

            SetEntityMask(CharacterMask_Usable);
        }

        Bridge2::~Bridge2()
        {
        }

        const App::Cursor* Bridge2::GetInteractCursor() const
        {
            return _buildCursor;
        }

        const HUD::Panel* Bridge2::GetTooltip() const
        {
            if (_tooltipDirty)
            {
                _tooltip = HUD::CreateBridgeTooltip(_contentManager, GetLevel()->GetStringTable(), _remainingCost);
                _tooltipDirty = false;
            }

            assert(_tooltip);
            return _tooltip;
        }

        float Bridge2::GetBuildPercentage() const
        {
            return _buildPerc;
        }

        bool Bridge2::IsBuilt() const
        {
            return _buildPerc >= 1.0f;
        }

        float Bridge2::GetGoldCostPerSecond(float buildSpeedMultiplier) const
        {
            return (_totalCost.Gold / TotalBuildTime) * buildSpeedMultiplier;
        }

        void Bridge2::Build(const Item::Resources& payment, float speedMultiplier)
        {
            _buildPercPaidFor = payment.Gold / static_cast<float>(_totalCost.Gold);
            _buildSpeedMultiplier = speedMultiplier;
            _remainingCost -= payment;
            _tooltipDirty = true;
        }

        bool Bridge2::CanBeInteractedWith(const Character* interactor) const
        {
            return _buildPerc < 1.0f && IsA<Builder>(interactor);
        }

        Vector2f Bridge2::GetInteractionMoveTarget(const Character* interactor) const
        {
            if (Vector2f::DistanceSquared(interactor->GetPosition(), _a.position) < Vector2f::DistanceSquared(interactor->GetPosition(), _b.position))
            {
                return _a.position;
            }
            else
            {
                return _b.position;
            }
        }

        Vector2f Bridge2::GetLineOfSightPoint() const
        {
            return (_a.endPiece->GetDrawBounds().Middle() + _b.endPiece->GetDrawBounds().Middle()) * 0.5f;
        }

        void Bridge2::OnSpawn()
        {
            Character::OnSpawn();
        }

        void Bridge2::OnLoadContent(Content::ContentManager* contentManager)
        {
            Character::OnLoadContent(contentManager);

            _contentManager = contentManager;
            SafeAddRef(_contentManager);

            _buildCursor = EmplaceResource(Content::GetCursor(contentManager, BuildCursorPath, "cursor_build"));

            ResourcePointer<Animation::SkeletonInstance> tmpLink =
                EmplaceResource(Content::CreateSkeletonInstance(contentManager, BridgeLinkSkeletonPath, BridgeLinkMatsetPath));

            _a.endPiece = EmplaceResource(Content::CreateSkeletonInstance(contentManager, BridgeEndSkeletonPath, BridgeEndMatsetPath));
            _a.endPiece->SetPosition(_a.position);
            _a.endPiece->SetInvertedX(true);

            _b.endPiece = EmplaceResource(Content::CreateSkeletonInstance(contentManager, BridgeEndSkeletonPath, BridgeEndMatsetPath));
            _b.endPiece->SetPosition(_b.position);
            _b.endPiece->SetInvertedX(false);

            float linkWidth = Vector2f::Distance(tmpLink->GetJointPosition("left_link"), tmpLink->GetJointPosition("right_link"));
            float gapWidth = Vector2f::Distance(_a.endPiece->GetJointPosition("bridge_connect"), _b.endPiece->GetJointPosition("bridge_connect"));

            uint32_t numLinks = uint32_t(Ceiling(gapWidth / linkWidth));
            float bridgeWidth = Max(numLinks * linkWidth, 1.0f);
            float bridgeScale = gapWidth / bridgeWidth;

            std::vector<Vector2f> linkChain =
                Graphics::ComputeRopePositions(_a.endPiece->GetJointPosition("bridge_connect"), _b.endPiece->GetJointPosition("bridge_connect"), numLinks, -Vector2f::UnitY, 1.25f).Points();
            std::vector<Vector2f> ropeChain =
                Graphics::ComputeRopePositions(_a.endPiece->GetJointPosition("rope_connect"), _b.endPiece->GetJointPosition("rope_connect"), numLinks * RopeLinkMultiplier, -Vector2f::UnitY, 1.25f).Points();

            uint32_t splitPoint = (numLinks / 2) + Random::RandomBetween(-1, 1);
            _a.bridgeChain = std::vector<Vector2f>(linkChain.begin(), linkChain.begin() + splitPoint + 1);
            _b.bridgeChain = std::vector<Vector2f>(linkChain.rbegin(), linkChain.rbegin() + (linkChain.size() - splitPoint));

            uint32_t ropeSplitPoint = splitPoint * RopeLinkMultiplier;
            _a.ropeChain = std::vector<Vector2f>(ropeChain.begin(), ropeChain.begin() + ropeSplitPoint + 1);
            _b.ropeChain = std::vector<Vector2f>(ropeChain.rbegin(), ropeChain.rbegin() + (ropeChain.size() - ropeSplitPoint));

            auto loadSide = [&](bridgeSide& side, bool invert)
            {
                side.bridgeCollision = MakeResource<Physics::RopeCollision>(GetLevelLayer()->GetPhysicsWorld(), side.bridgeChain, 40.0f);
                side.bridgeCollision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);

                uint32_t numSideLinks = side.bridgeChain.Size() - 1;
                for (uint32_t i = 0; i < numSideLinks; i++)
                {
                    ResourcePointer<Animation::SkeletonInstance> slat =
                        EmplaceResource(Content::CreateSkeletonInstance(contentManager, BridgeLinkSkeletonPath, BridgeLinkMatsetPath));
                    slat->SetScale(bridgeScale);
                    side.bridgeSlats.push_back(slat);

                    side.linkRopes.push_back(MakeResource<Graphics::RopeDrawable>(contentManager));
                }

                side.ropeCollision = MakeResource<Physics::RopeCollision>(GetLevelLayer()->GetPhysicsWorld(), side.ropeChain, 20.0f);
                side.ropeCollision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);
                side.ropeDrawable = MakeResource<Graphics::RopeDrawable>(contentManager);
            };

            loadSide(_a, false);

            loadSide(_b, true);
        }

        void Bridge2::OnUnloadContent()
        {
            Character::OnUnloadContent();

            SafeRelease(_contentManager);

            _buildCursor.Reset();
            _tooltip.Reset();

            auto unloadSide = [&](bridgeSide& side)
            {
                side.endPiece = nullptr;
                side.bridgeCollision = nullptr;
                side.bridgeSlats.clear();
                side.ropeCollision = nullptr;
                side.ropeDrawable = nullptr;
            };
            unloadSide(_a);
            unloadSide(_b);
        }

        void Bridge2::OnUpdate(double totalTime, float dt)
        {
            Character::OnUpdate(totalTime, dt);

            if (_buildPercPaidFor > 0.0f)
            {
                float buildPercThisTick = Max((dt / TotalBuildTime) * _buildSpeedMultiplier, _buildPercPaidFor);
                _buildPercPaidFor -= buildPercThisTick;
                _buildPerc += buildPercThisTick;
            }

            float ropeBuildPerc = Saturate(_buildPerc * 4.0f);

            auto updateSideSkeletons = [&](bridgeSide& side)
            {
                side.endPiece->SetColor(GetColor());
                side.endPiece->Update(totalTime, dt);
                Rectanglef bounds = side.endPiece->GetDrawBounds();

                side.ropeDrawable->SetColor(GetColor());

                float buildPercPerSlat = 1.0f / side.bridgeSlats.size();
                for (uint32_t slatIdx = 0; slatIdx < side.bridgeSlats.size(); slatIdx++)
                {
                    auto slat = side.bridgeSlats[slatIdx];

                    slat->SetColor(GetColor());

                    slat->Update(totalTime, dt);

                    bounds = Rectanglef::Merge(bounds, slat->GetDrawBounds());

                    float slatBuildPerc = Saturate((_buildPerc - (slatIdx * buildPercPerSlat)) / buildPercPerSlat);

                    Color baseColor = GetColor();
                    baseColor.A = Lerp(uint8_t(0), baseColor.A, slatBuildPerc);
                    side.linkRopes[slatIdx]->SetColor(baseColor);
                }

                return bounds;
            };

            auto updateSide = [&](bridgeSide& side)
            {
                side.bridgeCollision->Update(totalTime, dt);

                side.ropeCollision->Update(totalTime, dt);
                side.ropeCollision->SetLinkPosition(0, side.ropeChain[0]);

                if (ropeBuildPerc > 0.0f)
                {
                    Chainf ropeChain = side.ropeCollision->GetChain();
                    for (uint32_t i = 0; i < ropeChain.Size(); i++)
                    {
                        side.ropeCollision->SetLinkPosition(i, Vector2f::Lerp(ropeChain[i], side.ropeChain[i], ropeBuildPerc));
                    }
                }
                Chainf ropeChain = side.ropeCollision->GetChain();
                side.ropeDrawable->SetChain(ropeChain);

                float buildPercPerSlat = 1.0f / side.bridgeSlats.size();

                side.bridgeCollision->SetLinkPosition(0, side.bridgeChain[0]);
                for (uint32_t slatIdx = 0; slatIdx < side.bridgeSlats.size(); slatIdx++)
                {
                    auto slat = side.bridgeSlats[slatIdx];

                    float slatBuildPerc = Saturate((_buildPerc - (slatIdx * buildPercPerSlat)) / buildPercPerSlat);
                    if (slatBuildPerc > 0.0f)
                    {
                        side.bridgeCollision->SetLinkPosition(slatIdx + 1,
                            Vector2f::Lerp(side.bridgeCollision->GetLinkPosition(slatIdx + 1), side.bridgeChain[slatIdx + 1], slatBuildPerc * slatBuildPerc));
                    }

                    Vector2f a = side.bridgeCollision->GetLinkPosition(slatIdx);
                    Vector2f b = side.bridgeCollision->GetLinkPosition(slatIdx + 1);

                    Rayf attachPosition(a, b - a);

                    Animation::AttachmentInfo attachInfo =
                        Animation::GetSkeletonAttachmentInfo(false, false, true, attachPosition, slat, "left_link", "right_link");
                    slat->SetRotation(attachInfo.Rotation);
                    slat->SetScale(attachInfo.Scale);
                    slat->SetPosition(attachInfo.Position);
                    slat->SetInvertedX(attachInfo.InvertX);
                    slat->SetInvertedY(attachInfo.InvertY);

                    Chainf slatRopeChain;
                    slatRopeChain.AddPoint(slat->GetJointPosition("origin"));
                    //slatRopeChain.AddPoint(ropeChain[(slatIdx * RopeLinkMultiplier) + 1]);
                    slatRopeChain.AddPoint((ropeChain[slatIdx * RopeLinkMultiplier] + ropeChain[(slatIdx + 1) * RopeLinkMultiplier]) * 0.5f);
                    side.linkRopes[slatIdx]->SetChain(slatRopeChain);
                }
            };

            if (_pathId == Pathfinding::PathItemID_Invalid)
            {
                updateSide(_a);
                updateSide(_b);
            }
            _bounds = Rectanglef::Merge(updateSideSkeletons(_a), updateSideSkeletons(_b));

            if (_pathId == Pathfinding::PathItemID_Invalid && _buildPerc >= 1.0f)
            {
                Chainf pathChain;
                pathChain.AddPoint(_a.position);

                for (uint32_t i = 0; i < _a.bridgeSlats.size(); i++)
                {
                    const Vector2f& rightWalk = _a.bridgeSlats[i]->GetJointPosition("right_walk_top");

                    auto curSlat = _a.bridgeSlats[i];
                    auto nextSlat = i + 1 < _a.bridgeSlats.size() ? _a.bridgeSlats[i] : *_b.bridgeSlats.rbegin();

                    Vector2f leftWalk;
                    if (i + 1 < _a.bridgeSlats.size())
                    {
                        leftWalk = _a.bridgeSlats[i + 1]->GetJointPosition("left_walk_top");
                    }
                    else
                    {
                        leftWalk = _b.bridgeSlats.back()->GetJointPosition("right_walk_bottom");
                    }

                    pathChain.AddPoint((rightWalk + leftWalk) * 0.5f);
                }

                for (uint32_t i = 0; i < _b.bridgeSlats.size() - 1; i++)
                {
                    auto curSlat = *(_b.bridgeSlats.rbegin() + i);
                    auto nextSlat = *(_b.bridgeSlats.rbegin() + i + 1);

                    const Vector2f& rightWalk = curSlat->GetJointPosition("left_walk_bottom");
                    const Vector2f& leftWalk = nextSlat->GetJointPosition("right_walk_bottom");

                    pathChain.AddPoint((rightWalk + leftWalk) * 0.5f);
                }

                pathChain.AddPoint(_b.position);

                _pathId = GetLevelLayer()->GetPathSystem()->AddChain(pathChain, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::Wood);

                SetEntityMask(GetEntityMask() & ~CharacterMask_Usable);

                auto makeSideStatic = [&](bridgeSide& side)
                {
                    side.bridgeCollision = nullptr;
                    side.ropeCollision = nullptr;
                };

                makeSideStatic(_a);
                makeSideStatic(_b);
            }
        }

        void Bridge2::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Character::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(this, false, true, GetMinimumColor(), GetEmissiveColor(), GetHighlightColor());
        }

        const Rectanglef& Bridge2::GetBounds() const
        {
            return _bounds;
        }

        bool Bridge2::Intersects(const Vector2f& pt) const
        {
            if (!Rectanglef::Contains(_bounds, pt))
            {
                return false;
            }

            auto interects = [&](const bridgeSide& side)
            {
                if (side.endPiece->ContainsPoint(pt))
                {
                    return true;
                }

                if (side.ropeDrawable->ContainsPoint(pt))
                {
                    return true;
                }

                for (const auto& slat : side.bridgeSlats)
                {
                    if (slat->ContainsPoint(pt))
                    {
                        return true;
                    }
                }

                for (const auto& linkRope : side.linkRopes)
                {
                    if (linkRope->ContainsPoint(pt))
                    {
                        return true;
                    }
                }

                return false;
            };

            return interects(_a) || interects(_b);
        }

        void Bridge2::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            auto drawSide = [&](const bridgeSide& side)
            {
                for (auto& slatRope : side.linkRopes)
                {
                    slatRope->Draw(spriteRenderer);
                }

                side.ropeDrawable->Draw(spriteRenderer);

                for (auto& slat : side.bridgeSlats)
                {
                    slat->Draw(spriteRenderer);
                }

                side.endPiece->Draw(spriteRenderer);
            };

            drawSide(_a);
            drawSide(_b);
        }

        const Rectanglef& Bridge2::GetDrawBounds() const
        {
            return _bounds;
        }
    }

    template <>
    void EnumeratePreloads<Character::Bridge>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Character>(preloads);
        EnumeratePreloads<Graphics::BridgeDrawable>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::Bridge2>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Character>(preloads);

        preloads.insert(Character::BuildCursorPath);

        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
        preloads.insert(Character::BridgeEndSkeletonPath);
        preloads.insert(Character::BridgeEndMatsetPath);
        preloads.insert(Character::BridgeLinkSkeletonPath);
        preloads.insert(Character::BridgeLinkMatsetPath);
    }
}
