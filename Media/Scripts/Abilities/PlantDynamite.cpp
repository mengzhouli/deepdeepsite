#include "Abilities/PlantDynamite.hpp"

#include "Characters/Dynamite.hpp"
#include "Characters/CharacterTraits.hpp"

#include "NavigationUtility.hpp"
#include "AbilityUtility.hpp"
#include "ContentUtility.hpp"
#include "SkeletonUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string PlantDynamiteIconPath = "HUD/abilities.hudmatset";
        static const std::string PlantDynamiteCursorPath = "HUD/abilities.cursorset";

        static const std::string PlantDynamiteDynamiteSkeletonPath = "Skeletons/Characters/Pkeg/Pkeg.skel";
        static const std::string PlantDynamiteDynamiteMatsetPath = "Skeletons/Characters/Pkeg/Pkeg.polymatset";

        static const Audio::SoundPathVector PlantDynammitePlacementSounds = 
        {
            { "Audio/Characters/Pkeg/Keg_Placement.wav", 1.0f },
        };
        static const std::pair<float, float> PlantDynamitePlacementSoundRadius = { 1000.0f, 3000.0f };
        static const float PlantDynamitePlacementSoundVolume = 1.0f;

        PlantDynamite::PlantDynamite(const AbilityParameters& parameters, float fuseLength, float dmg, float radius, bool drawPlacementKeg)
            : BasicAbility(parameters, "ability_plant_dynamite_name", AbilityType_OneTarget)
            , _executing(false)

            , _validPosition(false)
            , _drawPlacementDynamiteEnabled(drawPlacementKeg)
            , _drawPlacementDynamite(false)

            , _fuseLength(fuseLength)
            , _radius(radius)
            , _dmg(dmg)
            , _placementDynamite(NULL)
        {
            SetIcon(PlantDynamiteIconPath, "icon_plant_dynamite");
            SetCursor(PlantDynamiteCursorPath, "cursor_plant_dynamite");
            SetCooldown(30.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_plant_dynamite_tooltip", _fuseLength, _dmg);

            _placementSounds.AddSounds(PlantDynammitePlacementSounds);
        }

        PlantDynamite::PlantDynamite(const AbilityParameters& parameters)
            : PlantDynamite(parameters, 4.0f, 60.0f, 800.0f, true)
        {
        }

        void PlantDynamite::ClearState()
        {
            _executing = false;
            _validPosition = false;
            setDrawPlacementDynamite(false);
            setDrawActionDynamite(false);
        }

        void PlantDynamite::SetPrimaryTarget(const Vector2f& pos)
        {
            Vector2f validPos;
            _validPosition = VerifyDynamitePosition(pos, validPos);
            _placementDynamite->SetPosition(validPos);
            setDrawPlacementDynamite(true);
        }

        bool PlantDynamite::IsInValidState() const
        {
            return _validPosition;
        }

        Character::Action PlantDynamite::GenerateAction() const
        {
            if (_validPosition)
            {
                return Character::CreateUseAbilityAction(GetID(), _placementDynamite->GetPosition());
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void PlantDynamite::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();
            _placementDynamite->SetPosition(Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY));
            _executing = owner->MoveTo(_placementDynamite->GetPosition());
        }

        void PlantDynamite::CancelAction()
        {
            ClearState();
        }

        bool PlantDynamite::IsExecutingAction() const
        {
            return _executing;
        }

        void PlantDynamite::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            Character::Character* owner = GetOwner();
            Character::Brewer* brewer = GetOwner<Character::Brewer>();
            assert(brewer);
            Rayf ownerAttachRay = owner->GetAttachPoint(brewer->GetKegAttachPointName());
            Animation::AttachmentInfo attachInfo = Animation::GetSkeletonAttachmentInfo(
                Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), false, ownerAttachRay,
                _actionDynamite, "attach_bottom", "attach_top");

            _actionDynamite->SetInvertedX(attachInfo.InvertX);
            _actionDynamite->SetInvertedY(attachInfo.InvertY);
            _actionDynamite->SetPosition(attachInfo.Position);
            _actionDynamite->SetScale(attachInfo.Scale);
            _actionDynamite->SetRotation(attachInfo.Rotation);
            _actionDynamite->Update(totalTime, dt);

            _placementDynamite->SetColor(GetAbilityUIColor(_validPosition));
            _placementDynamite->Update(totalTime, dt);

            if (_executing)
            {
                if (_dynamiteDropAnimationTimer >= 0.0f)
                {
                    _dynamiteDropAnimationTimer -= dt;
                    _dynamiteSpawnTimer -= dt;
                    _dynamiteDropTimer -= dt;

                    if (_dynamiteDropAnimationTimer < 0.0f)
                    {
                        StartCooldown();
                        _executing = false;
                    }
                    else if (_dynamiteDropTimer < 0.0f)
                    {
                        if (_drawActionDynamite)
                        {
                            setDrawActionDynamite(false);

                            Vector2f spawnPos;
                            VerifyDynamitePosition(_actionDynamite->GetPosition(), spawnPos);

                            // spawn dynamite
                            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                            Character::CharacterConstructor<Character::Dynamite> constructor =
                                Character::BindCharacterConstructor<Character::Dynamite>(owner->GetID(), _fuseLength, _radius, _dmg);
                            Character::Dynamite* dynamite = layer->SpawnCharacter(spawnPos, "dynamite", nullptr, constructor);
                            dynamite->SetRotation(_actionDynamite->GetRotation());
                            dynamite->SetScale(_actionDynamite->GetScale());

                            auto soundManager = layer->GetLevel()->GetSoundManager();
                            soundManager->PlaySinglePositionalSound(_placementSounds.GetNextSound(), Audio::SoundPriority::High, spawnPos,
                                                                    PlantDynamitePlacementSoundRadius.first, PlantDynamitePlacementSoundRadius.second,
                                                                    PlantDynamitePlacementSoundVolume);
                        }
                    }
                    else if (_dynamiteSpawnTimer < 0.0f)
                    {
                        if (!_drawActionDynamite)
                        {
                            setDrawActionDynamite(true);
                        }
                    }
                }
                else
                {
                    const float minDropDist = owner->GetBounds().W * 0.5f;
                    bool closeEnoughToDropPos = Vector2f::Distance(owner->GetPosition(), _placementDynamite->GetPosition()) < minDropDist;

                    if (!owner->IsMoving() || closeEnoughToDropPos)
                    {
                        owner->StopMoving();

                        auto info = brewer->PlayKegThrowdownAnimation();
                        _dynamiteDropAnimationTimer = info.TotalDuration;
                        _dynamiteSpawnTimer = info.KegSpawnTime;
                        _dynamiteDropTimer = info.KegDropTime;
                    }
                }
            }
        }

        void PlantDynamite::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _placementDynamite = Content::CreateSkeletonInstance(contentManager, PlantDynamiteDynamiteSkeletonPath, PlantDynamiteDynamiteMatsetPath);
            _actionDynamite = Content::CreateSkeletonInstance(contentManager, PlantDynamiteDynamiteSkeletonPath, PlantDynamiteDynamiteMatsetPath);

            _placementSounds.LoadContent(contentManager);
        }

        void PlantDynamite::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_placementDynamite);
            SafeRelease(_actionDynamite);

            _placementSounds.UnloadContent();
        }

        void PlantDynamite::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            if (_drawPlacementDynamite && _drawPlacementDynamiteEnabled)
            {
                levelRenderer->AddDrawable(_placementDynamite, false, false);
            }
            // action dynamite drawn as sub-drawable of owner
        }

        void PlantDynamite::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_drawPlacementDynamite && _validPosition && _drawPlacementDynamiteEnabled)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                Vector2f hudCenter = Camera::Transpose(worldCam, _placementDynamite->GetPosition(), hudCam);
                float hudRadius = Vector2f::Distance(hudCenter, Camera::Transpose(worldCam, _placementDynamite->GetPosition() + Vector2f(_radius, 0.0f), hudCam));

                const uint32_t segmentCount = 128;

                Color innerColor = Color::Red;
                innerColor.A = 63;
                Graphics::DrawFilledCircle(spriteRenderer, hudCenter, 0.0f, innerColor, hudRadius, innerColor, segmentCount);

                Color outterColor(innerColor.R, innerColor.G, innerColor.B, 127);
                Graphics::DrawCircle(spriteRenderer, hudCenter, hudRadius, outterColor, 2.0f, segmentCount);
            }
        }

        bool PlantDynamite::VerifyDynamitePosition(const Vector2f& pos, Vector2f& placePos)
        {
            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();
            if (Level::GetPlaceTarget(layer, Pathfinding::EdgeType_Walk, pos, 400.0f, placePos))
            {
                return true;
            }
            else
            {
                placePos = pos + (_placementDynamite->GetPosition() - _placementDynamite->GetDrawBounds().Middle());
                return false;
            }
        }

        void PlantDynamite::setDrawPlacementDynamite(bool drawPlacement)
        {
            _drawPlacementDynamite = drawPlacement;
        }

        void PlantDynamite::setDrawActionDynamite(bool drawAction)
        {
            Character::Character* owner = GetOwner<Character::Character>();
            Character::Brewer* brewer = GetOwner<Character::Brewer>();
            assert(brewer != nullptr);

            if (_drawActionDynamite != drawAction)
            {
                std::string attachName = brewer->GetKegAttachPointName();
                if (drawAction)
                {
                    owner->AttachDrawable(attachName, _actionDynamite);
                }
                else
                {
                    owner->DetachDrawable(attachName, _actionDynamite);
                }

                _drawActionDynamite = drawAction;
            }
        }
    }

    template <>
    void EnumeratePreloads<Ability::PlantDynamite>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);
        preloads.insert(Ability::PlantDynamiteIconPath);
        preloads.insert(Ability::PlantDynamiteCursorPath);
        EnumeratePreloads<Character::Dynamite>(preloads);
        preloads.insert(Ability::PlantDynamiteDynamiteSkeletonPath);
        preloads.insert(Ability::PlantDynamiteDynamiteMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Ability::PlantDynammitePlacementSounds);
    }
}
