#include "Levels/MenuLevel.hpp"

#include "Character/Controller.hpp"
#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"
#include "Level/Components/SpriteTerrainLevelComponent.hpp"
#include "MathUtility.hpp"

#include "Settings/TheDeepDeepProfile.hpp"

#include "Characters/Torch.hpp"
#include "Characters/ResourceNode.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dummy.hpp"
#include "Characters/MrBones.hpp"
#include "Characters/Gobbo.hpp"

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/PickAxe.hpp"
#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Weapons/Gun.hpp"

#include "Items/Trinkets/Headlamp.hpp"

#include "Drawables/RopeDrawable.hpp"
#include "Levels/GameLevels.hpp"
#include "Levels/LoadoutLevel.hpp"

#include "ContentUtility.hpp"

#include "HUD/Tooltip.hpp"

#include <string>

namespace Dwarf
{
    namespace Item
    {
        static const std::string DirectorPointyStickSkeletonPath = "Skeletons/Items/Weapons/Swords/pointer.skel";
        static const std::string DirectorPointyStickMatsetPath = "Skeletons/Items/Weapons/Swords/pointer.polymatset";

        class DirectorPointyStick : public BasicMeleeWeapon
        {
        public:
            DirectorPointyStick(const ItemParameters& parameters)
                : BasicMeleeWeapon(parameters, "", WeaponType_Melee_1H, DirectorPointyStickSkeletonPath, DirectorPointyStickMatsetPath)
            {
                SetAttachPoints("origin", "b");
            }

            const Vector2f& GetPointerPosition() const
            {
                return GetSkeleton()->GetJointPosition("pointer");
            }
        };
    }

    template <>
    void EnumeratePreloads<Item::DirectorPointyStick>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);
        preloads.insert(Item::DirectorPointyStickSkeletonPath);
        preloads.insert(Item::DirectorPointyStickMatsetPath);
    }

    namespace Character
    {
        class DirectorDwarf : public NavigatorDwarf
        {
        public:
            DirectorDwarf(const CharacterParameters& parameters)
                : NavigatorDwarf(parameters)
                , _rope(nullptr)
                , _targetPosition()
                , _velocity()
                , _distanceToTarget()
            {
                SetMoveType(MoveType_None);
                DisablePhysics();
                SetInvulnerable(true);

                ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_All);
                AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_All, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "DANGLE", 1.0f);

                ClearWeaponAttachments();
                AddWeaponAttachment("handb", "weapon_b_0", "weapon_b_1", { { Item::WeaponType_Melee_1H, "handb" }, });
            }

            void TransitionToPosition(const Vector2f& pos)
            {
                _targetPosition = pos;
            }

            float GetDistanceToTarget() const
            {
                return _distanceToTarget;
            }

        protected:
            virtual void OnSpawn() override
            {
                BasicCharacter::OnSpawn();

                _targetPosition = GetPosition();

                GiveItem(Item::BindItemConstructor<Item::DirectorPointyStick>());
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                BasicCharacter::OnLoadContent(contentManager);

                _rope = new Graphics::RopeDrawable(contentManager);
            }

            virtual void OnUnloadContent() override
            {
                BasicCharacter::OnUnloadContent();

                SafeRelease(_rope);
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                BasicCharacter::OnUpdate(totalTime, dt);

                const Vector2f& pointerPosition = GetWeapons<Item::DirectorPointyStick>()[0]->GetPointerPosition();

                Vector2f pos = GetPosition();

                Vector2f targetDir = _targetPosition - pointerPosition;
                _distanceToTarget = Vector2f::Distance(_targetPosition, pointerPosition);

                float acceleration = 400.0f;
                _velocity += Vector2f::Normalize(_targetPosition - pointerPosition) * (acceleration * dt);

                const float slowdownStart = 300.0f;
                const float slowdownEnd = 50.0f;

                float maxVelocity = 800.0f;
                if (_distanceToTarget < slowdownStart)
                {
                    float perc = Clamp((_distanceToTarget - slowdownEnd) / (slowdownStart - slowdownEnd), 0.0f, 1.0f);
                    maxVelocity = perc * maxVelocity;
                }

                float vDotT = Vector2f::Dot(Vector2f::Normalize(_velocity), Vector2f::Normalize(targetDir));
                float velocityDampening = Lerp(3.0f, 1.0f, ((vDotT + 1.0f) / 2.0f));
                maxVelocity /= velocityDampening;

                if (_velocity.LengthSquared() > maxVelocity * maxVelocity)
                {
                    _velocity = Vector2f::Normalize(_velocity) * maxVelocity;
                }

                pos += (_velocity * dt);
                SetPosition(pos);

                float targetDirX = Sign(_targetPosition.X - pointerPosition.X) * (Max(Abs(_targetPosition.X - pointerPosition.X) - slowdownStart, 0.0f));

                Vector2f ropeDir = Vector2f::Normalize(Vector2f(Clamp(targetDirX * 0.0001f, -0.2f, 0.2f), -1.0f));
                Vector2f ropeAttachPos = GetAttachPoint("chest").Position;

                Chainf ropeChain;
                ropeChain.AddPoint(ropeAttachPos + (ropeDir * 10000.0f));
                ropeChain.AddPoint(ropeAttachPos);

                _rope->SetChain(ropeChain);


                Animation::SkeletonInstance* skeleton = GetSkeleton();
                skeleton->SetRotation(Rotatorf(Vector2f(-ropeDir.X, ropeDir.Y)) - Rotatorf::PiOver2);
            }

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override
            {
                levelRenderer->AddDrawable(_rope, GetSkeletonCastsShadows());

                BasicCharacter::OnDraw(levelRenderer);
            }

        private:
            Graphics::RopeDrawable* _rope;

            Vector2f _targetPosition;
            Vector2f _velocity;
            float _distanceToTarget;
        };
    }

    template <>
    void EnumeratePreloads<Character::DirectorDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Item::DirectorPointyStick>(preloads);
        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
    }

    namespace Character
    {
        static const std::string PullerDwarfSkeletonPath = "Skeletons/Characters/Dwarf2/dwarf2.skel";
        static const std::string PullerDwarfMatsetPath = "Skeletons/Characters/Dwarf2/navigator.polymatset";

        static const Audio::SoundPathVector RopePullSounds =
        {
            { "Audio/Characters/Pulley Rope/Pulley_Rope.wav", 1.0f },
        };

        class PullerDwarf : public NavigatorDwarf
        {
        public:
            PullerDwarf(const CharacterParameters& parameters, const DirectorDwarf* director)
                : NavigatorDwarf(parameters)
                , _rope(nullptr)
                , _director(director)
                , _pulling(false)
            {
                assert(_director);
                _director->AddRef();

                SetMoveType(MoveType_None);
                DisablePhysics();
                SetInvulnerable(true);

                _pullAnimations.AddAnimation("ROPE_PULL", 1.0f);
                _pullIdleAnimations.AddAnimation("ROPE_PULL_IDLE", 1.0f);

                _ropePullSounds.AddSounds(RopePullSounds);

                ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_All);
            }

        protected:
            virtual ~PullerDwarf()
            {
                SafeRelease(_director);
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                NavigatorDwarf::OnLoadContent(contentManager);

                _rope = new Graphics::RopeDrawable(contentManager);
                _ropePullSounds.LoadContent(contentManager);
            }

            virtual void OnUnloadContent() override
            {
                NavigatorDwarf::OnUnloadContent();

                SafeRelease(_rope);
                _ropePullSounds.UnloadContent();
            }

            virtual void OnUpdate(double totalTime, float dt) override
            {
                BasicCharacter::OnUpdate(totalTime, dt);

                const float minPullDistance = 300.0f;
                const bool shouldBePulling = _director->GetDistanceToTarget() > minPullDistance;
                if (shouldBePulling != _pulling)
                {
                    if (shouldBePulling)
                    {
                        PlayAnimationSet(_pullAnimations, true, 0.1f, 0.0f);

                        auto soundManager = GetLevel()->GetSoundManager();
                        _ropePullInstance = soundManager->PlayLoopingPositionalSound(_ropePullSounds.GetNextSound(), Audio::SoundPriority::Medium, GetPosition(), 1000.0f, 3000.0f, 0.5f);
                    }
                    else
                    {
                        PlayAnimationSet(_pullIdleAnimations, true, 0.1f, 0.0f);
                        if (_ropePullInstance)
                        {
                            _ropePullInstance->Stop(0.2f);
                        }
                    }
                    _pulling = shouldBePulling;
                }

                Vector2f ropeDir(-0.50f, -1.0f);
                Vector2f ropeDrop(0.0f, 200.0f);

                Vector2f ropeAttachPosA = GetAttachPoint("weapon_b_1").Position;
                Vector2f ropeAttachPosB = GetAttachPoint("weapon_a_1").Position;

                Chainf ropeChain;
                ropeChain.AddPoint(ropeAttachPosA + (ropeDir * 10000.0f));
                ropeChain.AddPoint(ropeAttachPosA);
                ropeChain.AddPoint(ropeAttachPosB);
                ropeChain.AddPoint(ropeAttachPosB + ropeDrop);
                _rope->SetChain(ropeChain);
            }

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override
            {
                levelRenderer->AddDrawable(_rope, GetSkeletonCastsShadows());

                NavigatorDwarf::OnDraw(levelRenderer);
            }

        private:
            Graphics::RopeDrawable* _rope;
            const DirectorDwarf* _director;
            bool _pulling;

            Animation::AnimationSet _pullAnimations;
            Animation::AnimationSet _pullIdleAnimations;

            Audio::SoundSet _ropePullSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _ropePullInstance;
        };
    }

    template <>
    void EnumeratePreloads<Character::PullerDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Graphics::RopeDrawable>(preloads);
        EnumerateSoundVectorPreloads(preloads, Character::RopePullSounds);
    }

    namespace Character
    {
        static const std::string CampaignBoardSkeletonPath = "Skeletons/UI/Levelmap/level_map.skel";
        static const std::string CampaignBoardMatsetPath = "Skeletons/UI/Levelmap/level_map.polymatset";

        struct LevelMapItem
        {
            std::string Level;
            std::vector<std::string> ClickableMaterials;
            std::vector<std::string> EnableMaterials;
            std::vector<std::string> DisableMaterials;
            std::string StartAnimation;
            std::string OpenAnimation;
        };

        static const auto CampaignLevelMapItems = []()
        {
            static const std::vector<LevelMapItem> items
            {
                { Level::Campaign::DwarfHomes0(), { "dwarf_homes_full", }, { "dwarf_homes_full", "path0_full", }, { "dwarf_homes_stencil", "path0_stencil", }, "map_0_start", "map_0", },
                { Level::Campaign::DwarfHomes1(),{ "dwarf_plaza_full", },{ "dwarf_plaza_full", "path1_full", }, { "dwarf_plaza_stencil", "path1_stencil", }, "map_1_start", "map_1", },
                { Level::Campaign::DwarfHomes2(),{ "towers_full", },{ "towers_full", "path3_full", },{ "towers_stencil", "path3_stencil", }, "map_2_start", "map_2", },
                { Level::Campaign::DwarfHomes3(),{ "mines_full", },{ "mines_full", "path4_full", },{ "mines_stencil", "path4_stencil", }, "map_3_start", "map_3", },
                { Level::Campaign::DwarfHomes4(),{ "forge_full", "anvil_full", },{ "forge_full", "anvil_full", "path5_full", },{ "forge_stencil", "anvil_stencil", "path5_stencil" }, "map_4_start", "map_4", },
            };
            return items;
        };

        static const std::string CampaignBoardLevelLayerName = "background_mine";

        class MenuController : public Controller
        {
        public:
            MenuController(const ControllerParameters& parameters, CampaignLoadoutConstructor campaignLoadoutConstructor, ChallengeLoadoutConstructor challengeLoadoutConstructor,
                           LevelChangeFunction levelChangeFunc, ExitGameFunction exitGameFunc, MenuState initialState)
                : Controller(parameters)
                , _profile(nullptr)
                , _leftRopeDwarf(nullptr)
                , _rightRopeDwarf(nullptr)
                , _directorDwarf(nullptr)
                , _context(initialState)
                , _leftLightsOn(false)
                , _rightLightsOn(false)
                , _menuUpBind(Input::InputBindCode_Invalid)
                , _menuDownBind(Input::InputBindCode_Invalid)
                , _menuLeftBind(Input::InputBindCode_Invalid)
                , _menuRightBind(Input::InputBindCode_Invalid)
                , _menuSelectBind(Input::InputBindCode_Invalid)
                , _menuBackBind(Input::InputBindCode_Invalid)
                , _exitGameFunction(exitGameFunc)
                , _levelChangeFunction(levelChangeFunc)
                , _campaignLoadoutConstructor(campaignLoadoutConstructor)
                , _challengeLoadoutConstructor(challengeLoadoutConstructor)
                , _menuFont(nullptr)
                , _mousePosScreen()
                , _cursor(nullptr)
            {
                _profile = GetLevel()->GetProfile<Settings::TheDeepDeepProfile>();
                _menuUpBind = _profile->GetBindCode("menu_up");
                _menuDownBind = _profile->GetBindCode("menu_down");
                _menuLeftBind = _profile->GetBindCode("menu_left");
                _menuRightBind = _profile->GetBindCode("menu_right");
                _menuSelectBind = _profile->GetBindCode("menu_select");
                _menuBackBind = _profile->GetBindCode("menu_back");
                _pauseBind = _profile->GetBindCode("pause");
            }

        protected:
            void followSpline(const std::string& splineName, float time, bool reverse)
            {
                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance* primaryLayer = level->GetPrimaryLayer();
                Level::CameraController& cameraController = level->GetCameraController();
                cameraController.FollowSpline(primaryLayer->GetSpline(splineName), time, reverse);

                _transitionTimer = 0.0f;
                _transitionDuration = time;
            }

            void transitionToState(MenuState state)
            {
                MenuState prevState = _context;
                _context = state;

                switch (state)
                {
                case Menu_Title:
                    if (prevState == Menu_Options)
                    {
                        followSpline("camera_move_title_to_menu", 5.0f, true);
                    }
                    else
                    {
                        followSpline("camera_move_title_to_menu", 0.0f, true);
                    }
                    break;
                case Menu_Options:
                    if (prevState == Menu_Title)
                    {
                        followSpline("camera_move_title_to_menu", 5.0f, false);
                    }
                    else if (prevState == Menu_Campaign)
                    {
                        followSpline("camera_move_menu_to_campaign", 3.0f, true);
                    }
                    else if (prevState == Menu_Challenges)
                    {
                        followSpline("camera_move_menu_to_challenge", 3.0f, true);
                    }
                    else
                    {
                        followSpline("camera_move_title_to_menu", 0.0f, false);
                    }
                    break;
                case Menu_Campaign:
                    if (prevState == Menu_Options)
                    {
                        followSpline("camera_move_menu_to_campaign", 3.0f, false);
                    }
                    else
                    {
                        followSpline("camera_move_menu_to_campaign", 0.0f, false);
                    }
                    TurnTorchesOnOff("torch_right.*", true);
                    _rightLightsOn = true;
                    break;
                case Menu_Challenges:
                    if (prevState == Menu_Options)
                    {
                        followSpline("camera_move_menu_to_challenge", 3.0f, false);
                    }
                    else
                    {
                        followSpline("camera_move_menu_to_challenge", 0.0f, false);
                    }
                    TurnTorchesOnOff("torch_left.*", true);
                    _leftLightsOn = true;
                    break;
                case Menu_Exit:
                    if (prevState == Menu_Options)
                    {
                        followSpline("camera_move_menu_to_exit", 8.0f, false);
                    }
                    else
                    {
                        followSpline("camera_move_menu_to_exit", 0.0f, false);
                    }
                    break;
                default:
                    break;
                }
            }

            virtual void OnCreate() override
            {
                Controller::OnCreate();
                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance* primaryLayer = level->GetPrimaryLayer();

                level->SetMinimapEnabled(false);

                CharacterConstructor<NavigatorDwarf> navigatorConstructor = BindCharacterConstructor<NavigatorDwarf>();

                _leftRopeDwarf = primaryLayer->SpawnCharacter(GetTriggerPosition("dwarf_spawn_left"), "leftRopeDwarf", this, navigatorConstructor);
                _leftRopeDwarf->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                _leftRopeDwarf->PushAction(CreateMoveAction(GetTriggerPosition("dwarf_move_left")), true);

                _rightRopeDwarf = primaryLayer->SpawnCharacter(GetTriggerPosition("dwarf_spawn_right"), "rightRopeDwarf", this, navigatorConstructor);
                _rightRopeDwarf->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                _rightRopeDwarf->PushAction(CreateMoveAction(GetTriggerPosition("dwarf_move_right")), true);
                _rightRopeDwarf->LookAt(GetTriggerPosition("dwarf_look_right"));

                _directorDwarf = primaryLayer->SpawnCharacter(GetTriggerPosition("director_dwarf_spawn"), "director", this, BindCharacterConstructor<DirectorDwarf>());

                _pullerDwarf = primaryLayer->SpawnCharacter(GetTriggerPosition("puller_dwarf_spawn"), "puller", this, BindCharacterConstructor<PullerDwarf>(_directorDwarf));
                _pullerDwarf->SetRotation(-Vector2f::UnitX);

                level->GetCameraController().FollowSpline(primaryLayer->GetSpline("camera_move_title_to_menu"), 0.0f, false);

                TurnTorchesOnOff("torch_left.*", false);
                _leftLightsOn = false;
                TurnTorchesOnOff("torch_right.*", false);
                _rightLightsOn = false;

                const Vector2f& campaignPosterPosition = GetLevel()->GetLayer(CampaignBoardLevelLayerName)->GetTriggerPosition("campaign_poster_spawn");
                _campaignPoster->SetPosition(campaignPosterPosition);

                transitionToState(_context);
                if (_context == Menu_Title)
                {
                    AsA<Level::MenuLevel>(GetLevel())->PlayOpeningSwell();
                }
            }

            virtual void OnUpdate(const Input::FrameInput& input, double totalTime, float dt) override
            {
                Controller::OnUpdate(input, totalTime, dt);

                SafeRelease(_curTooltip);

                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance* primaryLayer = level->GetPrimaryLayer();
                Level::CameraController& cameraController = level->GetCameraController();
                const Camera& worldCamera = cameraController.GetCamera();
                const Camera& hudCamera = level->GetHUDCamera();

                bool transitioning = cameraController.PerformingAction();
                _transitionTimer += dt;
                float transitionPerc = _transitionDuration > 0.0f ?  _transitionTimer / _transitionDuration : 1.0f;

                _mousePosScreen = input.UnProjectMousePosition(hudCamera);
                Vector2f mousePosWorld = input.UnProjectMousePosition(worldCamera);

                if (_context == Menu_Title)
                {
                    if (!transitioning)
                    {
                        if (input.IsBindJustPressed(_menuSelectBind))
                        {
                            transitionToState(Menu_Options);
                        }
                    }
                }
                else if (_context == Menu_Options)
                {
                    if (!transitioning)
                    {
                        if (_rightLightsOn)
                        {
                            TurnTorchesOnOff("torch_right.*", false);
                            _rightLightsOn = false;
                        }

                        if (_leftLightsOn)
                        {
                            TurnTorchesOnOff("torch_left.*", false);
                            _leftLightsOn = false;
                        }

                        if (input.IsBindJustPressed(_menuSelectBind))
                        {
                            if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_campaign"), mousePosWorld))
                            {
                                transitionToState(Menu_Campaign);
                            }
                            else if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_challenge"), mousePosWorld))
                            {
                                transitionToState(Menu_Challenges);
                            }
                            else if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_exit"), mousePosWorld))
                            {
                                transitionToState(Menu_Exit);
                            }
                        }

                        if (input.IsBindJustPressed(_menuBackBind))
                        {
                            transitionToState(Menu_Title);
                        }
                    }
                }
                else if (_context == Menu_Campaign)
                {
                    if (!transitioning)
                    {
                        const Polygonf& directorMoveArea = primaryLayer->GetTriggerArea("director_move_area");
                        if (Polygonf::Contains(directorMoveArea, mousePosWorld))
                        {
                            _directorDwarf->TransitionToPosition(mousePosWorld);
                        }

                        std::vector<BasicCharacter*> mouseLookCharacters
                        {
                            _directorDwarf,
                            _pullerDwarf,
                        };

                        for (auto mouseLookCharacter : mouseLookCharacters)
                        {
                            bool forward = mousePosWorld.X <= mouseLookCharacter->GetPosition().X;
                            if (forward)
                            {
                                mouseLookCharacter->LookAt(mousePosWorld);
                            }
                            else
                            {
                                mouseLookCharacter->StopLooking();
                            }
                        }

                        _campaignLights.clear();
                        for (auto campaignLevel : CampaignLevelMapItems())
                        {
                            if (_profile->IsCampaignLevelUnlocked(campaignLevel.Level))
                            {
                                bool levelSelected = false;
                                bool levelClicked = false;
                                for (const auto& clickableJoint : campaignLevel.ClickableMaterials)
                                {
                                    if (_campaignPoster->HasJoint(clickableJoint) &&
                                        _campaignPoster->JointContainsPoint(clickableJoint, mousePosWorld))
                                    {
                                        levelSelected = true;
                                        if (input.IsBindJustPressed(_menuSelectBind))
                                        {
                                            levelClicked = true;
                                        }
                                    }
                                }

                                if (levelSelected)
                                {
                                    for (const auto& clickableJoint : campaignLevel.ClickableMaterials)
                                    {
                                        Lights::PolygonMaterialLight light;
                                        light.Material = _campaignPoster->GetMaterial(clickableJoint);

                                        const Animation::Transformation& materialTransform = _campaignPoster->GetJointTransformation(clickableJoint);
                                        light.Position = materialTransform.Position;
                                        light.Rotation = materialTransform.Rotation;
                                        light.Scale = materialTransform.Scale.X;

                                        _campaignLights.push_back(light);
                                    }

                                    _curTooltip = _campaignLevelTooltips[campaignLevel.Level];
                                }

                                if (levelClicked)
                                {
                                    _levelChangeFunction(_campaignLoadoutConstructor(Level::GameCampaignLevel::GetInfo(campaignLevel.Level),
                                        Level::GameCampaignLevel::GetConstructor(campaignLevel.Level)));
                                    break;
                                }
                            }
                        }

                        if (!_justUnlockedLevels.empty())
                        {
                            for (const auto& justUnlockedLevel : _justUnlockedLevels)
                            {
                                for (const auto& campaignLevel : CampaignLevelMapItems())
                                {
                                    if (campaignLevel.Level == justUnlockedLevel)
                                    {
                                        _campaignPoster->PlayAnimation(campaignLevel.OpenAnimation, false);
                                    }
                                }
                                _profile->MarkCampaignLevelSeen(justUnlockedLevel);
                            }

                            _justUnlockedLevels.clear();
                        }

                        if (input.IsBindJustPressed(_menuBackBind))
                        {
                            transitionToState(Menu_Options);
                        }
                    }
                }
                else if (_context == Menu_Challenges)
                {
                    if (!transitioning)
                    {
                        if (input.IsBindJustPressed(_menuBackBind))
                        {
                            transitionToState(Menu_Options);
                        }
                    }
                }
                else if (_context == Menu_Exit)
                {
                    if (!transitioning || transitionPerc > 0.5f)
                    {
                        _exitGameFunction();
                    }
                }

                SafeRelease(_menuLight.Material);
                if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_campaign"), mousePosWorld))
                {
                    updateLightToDrawable(primaryLayer, "campaign_text", _menuLight);
                }
                else if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_challenge"), mousePosWorld))
                {
                    updateLightToDrawable(primaryLayer, "challenge_text", _menuLight);
                }
                else if (Polygonf::Contains(primaryLayer->GetTriggerArea("menu_exit"), mousePosWorld))
                {
                    updateLightToDrawable(primaryLayer, "exit_text", _menuLight);
                }

                const Vector2f& lookTop = GetTriggerPosition("dwarf_look_top");
                const Vector2f& lookBot = GetTriggerPosition("dwarf_look_bottom");

                Vector2f closestPt;
                Math::PointToLineDistance(lookTop, lookBot, mousePosWorld, closestPt);

                _leftRopeDwarf->LookAt(closestPt);

                _campaignPoster->Update(totalTime, dt);

                SafeAddRef(_curTooltip);
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                Controller::OnLoadContent(contentManager);

                _menuFont = contentManager->Load<Graphics::Font>("Fonts/menu_font.ttf");

                _cursor = HUD::CreateCursor(contentManager, "HUD/hud.cursorset", "cursor_regular");

                _campaignPoster = Content::CreateSkeletonInstance(contentManager, CampaignBoardSkeletonPath, CampaignBoardMatsetPath);
                _campaignPoster->PlayAnimation("idle", true, 0.0f, 0.0f);

                for (auto campaignLevel : CampaignLevelMapItems())
                {
                    bool justUnlocked = _profile->IsCampaignLevelJustUnlocked(campaignLevel.Level);
                    if (justUnlocked)
                    {
                        _justUnlockedLevels.insert(campaignLevel.Level);

                        _campaignPoster->PlayAnimation(campaignLevel.StartAnimation, false, 0.0f, 0.0f);
                    }

                    if (_profile->IsCampaignLevelUnlocked(campaignLevel.Level))
                    {
                        for (const auto& disableMaterial : campaignLevel.DisableMaterials)
                        {
                            _campaignPoster->ClearMaterial(disableMaterial);
                        }
                    }
                    else
                    {
                        for (const auto& enableMaterial : campaignLevel.EnableMaterials)
                        {
                            _campaignPoster->ClearMaterial(enableMaterial);
                        }
                    }

                    _campaignLevelTooltips[campaignLevel.Level] =
                        HUD::CreateCampaignLevelTooltip(contentManager, GetLevel()->GetStringTable(),
                                                        Level::GameCampaignLevel::GetInfo(campaignLevel.Level),
                                                        _profile);
                }
            }

            virtual void OnUnloadContent() override
            {
                Controller::OnUnloadContent();

                SafeRelease(_menuFont);
                SafeRelease(_cursor);
                SafeRelease(_campaignPoster);

                SafeRelease(_curTooltip);

                for (auto& tooltip : _campaignLevelTooltips)
                {
                    SafeRelease(tooltip.second);
                }
                _campaignLevelTooltips.clear();
            }

            virtual void OnDraw(Level::LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override
            {
                Controller::OnDraw(layer, levelRenderer);

                if (layer->GetName() == CampaignBoardLevelLayerName)
                {
                    levelRenderer->AddDrawable(_campaignPoster, false);
                    for (const auto& light : _campaignLights)
                    {
                        levelRenderer->AddLight(light);
                    }
                }

                if (layer->IsPrimary())
                {
                    levelRenderer->AddLight(_menuLight);
                    levelRenderer->SetCursor(_cursor);
                }
            }

            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override
            {
                Controller::OnDrawHUD(spriteRenderer);

                if (_curTooltip)
                {
                    const float uiScale = _profile->GetUIScale();
                    Vector2f minimumSize = _curTooltip->GetMinimumSize(uiScale);
                    Vector2u cursorSize = _cursor ? Vector2u(_cursor->Width(), _cursor->Height()) : Vector2u::Zero;
                    const float tooltipCursorPadding = 3.0f;
                    Rectanglef tooltipBounds(_mousePosScreen + Vector2f(0, cursorSize.Y + tooltipCursorPadding), minimumSize);
                    _curTooltip->Draw(spriteRenderer, Color::White, tooltipBounds, uiScale);
                }
            }

        private:
            static void updateLightToDrawable(const Level::LevelLayerInstance* layer, const std::string& name, Lights::RectangleMaterialLight& light)
            {
                const Level::SpriteTerrainLevelComponent* spriteTerrain = layer->GetDrawable<Level::SpriteTerrainLevelComponent>(name);
                assert(spriteTerrain != nullptr);

                assert(light.Material == nullptr);

                Graphics::RectangleMaterial* mat = new Graphics::RectangleMaterial(spriteTerrain->GetTexture(), spriteTerrain->GetSubRect(),
                                                                                   Vector2f(0.5f), spriteTerrain->GetRotation(), spriteTerrain->GetScale());
                light.Material = mat;
                light.Position = spriteTerrain->GetPosition();
                light.Rotation = 0.0f;
                light.Scale = 1.0f;
                light.EmissiveColor = Color::FromFloats(0.5f, 0.5f, 0.5f, 1.0f);
                light.LightColor = Color::White;
            }

            const Vector2f& GetTriggerPosition(const std::string& name)
            {
                return GetLevel()->GetPrimaryLayer()->GetTriggerPosition(name);
            }

            void TurnTorchesOnOff(const std::string& name, bool onOff)
            {
                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance* primaryLayer = level->GetPrimaryLayer();

                std::vector<Torch*> torches = primaryLayer->GetCharacters<Torch>(name);
                for (uint32_t i = 0; i < torches.size(); i++)
                {
                    if (onOff)
                    {
                        torches[i]->TurnOn();
                    }
                    else
                    {
                        torches[i]->TurnOff();
                    }
                }
            }

            Settings::TheDeepDeepProfile* _profile;

            NavigatorDwarf* _leftRopeDwarf;
            NavigatorDwarf* _rightRopeDwarf;

            std::set<std::string> _justUnlockedLevels;
            Animation::SkeletonInstance* _campaignPoster;
            std::vector<Lights::PolygonMaterialLight> _campaignLights;

            Lights::RectangleMaterialLight _menuLight;

            DirectorDwarf* _directorDwarf;
            PullerDwarf* _pullerDwarf;

            MenuState _context;

            bool _leftLightsOn;
            bool _rightLightsOn;

            Input::InputBindCode _pauseBind;
            Input::InputBindCode _menuUpBind;
            Input::InputBindCode _menuDownBind;
            Input::InputBindCode _menuLeftBind;
            Input::InputBindCode _menuRightBind;
            Input::InputBindCode _menuSelectBind;
            Input::InputBindCode _menuBackBind;

            ExitGameFunction _exitGameFunction;

            LevelChangeFunction _levelChangeFunction;

            CampaignLoadoutConstructor _campaignLoadoutConstructor;
            ChallengeLoadoutConstructor _challengeLoadoutConstructor;

            const Graphics::Font* _menuFont;

            Vector2f _mousePosScreen;
            const App::Cursor* _cursor;

            float _transitionTimer;
            float _transitionDuration;

            const HUD::Panel* _curTooltip = nullptr;
            std::map<std::string, const HUD::Panel*> _campaignLevelTooltips;
        };
    }

    template <>
    void EnumeratePreloads<Character::MenuController>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Controller>(preloads);

        EnumeratePreloads<Character::PullerDwarf>(preloads);
        EnumeratePreloads<Character::DirectorDwarf>(preloads);
        EnumeratePreloads<Character::NavigatorDwarf>(preloads);
        EnumeratePreloads<Character::Torch>(preloads);

        preloads.insert(Character::CampaignBoardSkeletonPath);
        preloads.insert(Character::CampaignBoardMatsetPath);

        preloads.insert("Fonts/menu_font.ttf");
        preloads.insert("HUD/hud.hudmatset");
        preloads.insert("HUD/hud.cursorset");
    }

    namespace Level
    {
        static const char* OpeningSwellName = "opening_swell";
        static const Audio::MusicSwellMap MenuMusicSwells = Audio::MusicSwellMap
        {
            {
                OpeningSwellName,
                {
                    "Music/IntroSwell/MusicIncidental_IntroSwell_B.ogg",
                    0.3f,
                },
            },
        };

        MenuLevel::MenuLevel(const LevelParameters& parameters, CampaignLoadoutConstructor campaignLoadoutConstructor, ChallengeLoadoutConstructor challengeLoadoutConstructor,
                             LevelChangeFunction levelChangeFunc, ExitGameFunction exitGameFunc, MenuState initialState)
            : BasicLevel(parameters)
            , _lightColors()
            , _danceLights(5)
            , _firstUpdate(true)
            , _fighters()
            , _fighterActionTimers()
            , _initialState(initialState)
            , _levelChangeFunction(levelChangeFunc)
            , _campaignLoadoutConstructor(campaignLoadoutConstructor)
            , _challengeLoadoutConstructor(challengeLoadoutConstructor)
            , _exitGameFunc(exitGameFunc)
        {
            _lightColors.push_back(Color::FromBytes(255, 128, 128, 255));
            _lightColors.push_back(Color::FromBytes(128, 255, 128, 255));
            _lightColors.push_back(Color::FromBytes(128, 128, 255, 255));
            _lightColors.push_back(Color::FromBytes(128, 255, 255, 255));
            _lightColors.push_back(Color::FromBytes(255, 128, 255, 255));
            _lightColors.push_back(Color::FromBytes(255, 255, 128, 255));

            AddMusicSwells(MenuMusicSwells);
        }

        void MenuLevel::PlayOpeningSwell()
        {
            PlayMusicSwell(OpeningSwellName);
        }

        void MenuLevel::OnCreate()
        {
            BasicLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::Cave);
            SetTriggerAreaEnvironmenType("audio_area_none", Audio::EnvironmentType::None);
            SetTriggerAreaEnvironmenType("audio_area_cave", Audio::EnvironmentType::Cave);
            SetTriggerAreaEnvironmenType("audio_area_stone_tunnel", Audio::EnvironmentType::StoneTunnel);
            SetTriggerAreaEnvironmenType("audio_area_stone_room", Audio::EnvironmentType::StoneRoom);
            SetTriggerAreaEnvironmenType("audio_area_wood_room", Audio::EnvironmentType::WoodRoom);

            SetDiscoveryEnabled(false);

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            Pathfinding::PathSystem* pathSystem = primaryLayer->GetPathSystem();

            Vector2f ropeTopLeft = primaryLayer->GetTriggerPosition("rope_top_left");
            Vector2f ropeBotLeft = primaryLayer->GetTriggerPosition("rope_bot_left");
            pathSystem->AddLine(ropeBotLeft, ropeTopLeft, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope);

            Vector2f ropeTopRight = primaryLayer->GetTriggerPosition("rope_top_right");
            Vector2f ropeBotRight = primaryLayer->GetTriggerPosition("rope_bot_right");
            pathSystem->AddLine(ropeBotRight, ropeTopRight, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope);

            SpawnCharactersAtTriggers("torch_spawn_left", Character::BindCharacterConstructor<Character::Torch>(BaseFireColor, false, true, true), "torch_left", NULL);
            SpawnCharactersAtTriggers("torch_spawn_right", Character::BindCharacterConstructor<Character::Torch>(BaseFireColor, false, true, true), "torch_right", NULL);

            SpawnCharactersAtTriggers("brazier_wall_spawn", Character::BindCharacterConstructor<Character::WallBrazier>(BaseFireColor, false, true, true), "wall_brazier", NULL);
            SpawnCharactersAtTriggers("brazier_floor_spawn", Character::BindCharacterConstructor<Character::PotBrazier>(BaseFireColor, false, true, true), "floor_brazier", NULL);
            SpawnCharactersAtTriggers("brazier_hang_spawn", Character::BindCharacterConstructor<Character::HangingBrazier>(BaseFireColor, false, true, true), "hang_brazier", NULL);

            Character::CharacterConstructor<Character::ResourceNode> oreConstructor = 
                Character::BindCharacterConstructor<Character::ResourceNode>("Skeletons/Characters/skellynode/skellynode.skel",
                                                                             "Skeletons/Characters/skellynode/skellynode.polymatset",
                                                                             Item::Resources(std::numeric_limits<int32_t>::max()));
            std::vector<Character::ResourceNode*> ores = SpawnCharactersAtTriggers("ore_spawn", oreConstructor, "ore", nullptr);

            std::vector< Item::ItemConstructor<Item::PickAxe> > pickAxes =
            {
                Item::BindItemConstructor<Item::WoodPickAxe>(),
                Item::BindItemConstructor<Item::IronPickAxe>(),
                Item::BindItemConstructor<Item::GoldPickAxe>(),
            };

            std::vector<Character::MinerDwarf*> miners = SpawnCharactersAtTriggers("miner_spawn", Character::BindCharacterConstructor<Character::MinerDwarf>(), "miner", nullptr);
            for (Character::MinerDwarf* miner : miners)
            {
                miner->GiveItem(Random::RandomItem(pickAxes));

                Character::ResourceNode* closestOre = nullptr;
                float closestOreDist = std::numeric_limits<float>::max();
                for (Character::ResourceNode* ore : ores)
                {
                    float distToOre = Vector2f::Distance(miner->GetPosition(), ore->GetPosition());
                    if (distToOre < closestOreDist && miner->CanMoveTo(ore->GetPosition()))
                    {
                        closestOre = ore;
                        closestOreDist = distToOre;
                    }
                }

                if (closestOre)
                {
                    miner->PushAction(Character::CreateInteractCharacterAction(closestOre->GetID()), false);
                }

                SetTargetCameraViewSize(2700.0f, 16.0f / 9.0f);
            }

            _dummies = SpawnCharactersAtTriggers<Character::Character>("dummy_spawn", Character::BindCharacterConstructor<Character::Dummy>(), "dummy", nullptr);
            for (Character::Character* dummy : _dummies)
            {
                dummy->SetMaxHealth(std::numeric_limits<float>::max());
                dummy->SetHealthRegeneration(std::numeric_limits<float>::max());
            }

            std::vector< Item::ItemConstructor<Item::Weapon> > mainHandSword =
            {
                Item::BindItemConstructor<Item::WoodenShortSword>(),
            };
            std::vector< Item::ItemConstructor<Item::Weapon> > offhandSword =
            {
                Item::BindItemConstructor<Item::WoodenBasicShield>(),
                Item::BindItemConstructor<Item::WoodenShortSword>(),
            };

            std::vector<Character::Character*> fighters = SpawnCharactersAtTriggers<Character::Character>("fighter_spawn", Character::BindCharacterConstructor<Character::FighterDwarf>(), "fighter", nullptr);
            for (Character::Character* fighter : fighters)
            {
                fighter->GiveItem(Random::RandomItem(mainHandSword));
                fighter->GiveItem(Random::RandomItem(offhandSword));
            }

            std::vector< Item::ItemConstructor<Item::Weapon> > guns =
            {
                Item::BindItemConstructor<Item::BasicRifle>(),
            };

            std::vector<Character::Character*> riflemen = SpawnCharactersAtTriggers<Character::Character>("rifleman_spawn", Character::BindCharacterConstructor<Character::BrewerDwarf>(), "fighter", nullptr);
            for (Character::Character* rifleman : riflemen)
            {
                rifleman->GiveItem(Random::RandomItem(guns));
            }

            _fighters.insert(_fighters.end(), fighters.begin(), fighters.end());
            _fighters.insert(_fighters.end(), riflemen.begin(), riflemen.end());
            _fighterActionTimers.resize(_fighters.size(), 0.0f);

            SpawnCharactersAtTriggers("gobbo_spawn", Character::BindCharacterConstructor<Character::Gobbo>(), "gobbo", nullptr);
            SpawnCharactersAtTriggers("skeleton_spawn", Character::BindCharacterConstructor<Character::MrBonesMelee>(), "skeleton", nullptr);

            CreateController("controller", Character::BindControllerConstructor<Character::MenuController>(_campaignLoadoutConstructor,
                                                                                                           _challengeLoadoutConstructor,
                                                                                                           _levelChangeFunction,
                                                                                                           _exitGameFunc,
                                                                                                           _initialState));
        }

        void MenuLevel::OnUpdate(double totalTime, float dt)
        {
            BasicLevel::OnUpdate(totalTime, dt);

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();

            Vector2f danceLightMid = primaryLayer->GetTriggerPosition("dance_light_middle");

            Vector2f danceLightRadiusStartPoint = primaryLayer->GetTriggerPosition("dance_light_radius_start");
            float danceLightRadiusStart = Vector2f::Distance(danceLightMid, danceLightRadiusStartPoint);

            Vector2f danceLightRadiusEndPoint = primaryLayer->GetTriggerPosition("dance_light_radius_end");
            float danceLightRadiusEnd = Vector2f::Distance(danceLightMid, danceLightRadiusEndPoint);

            Rotatorf lightArc((1.0f / _danceLights.size()) * Rotatorf::TwoPi.Angle * 0.75f);

            for (uint32_t i = 0; i < _danceLights.size(); i++)
            {
                _danceLights[i].Direction = Rotatorf(totalTime + (float(i) / _danceLights.size()) * Rotatorf::TwoPi.Angle);
                _danceLights[i].Position = danceLightMid;
                _danceLights[i].Arc = lightArc;
                _danceLights[i].StartRadius = danceLightRadiusStart;
                _danceLights[i].EndRadius = danceLightRadiusEnd;
                _danceLights[i].LightColor = _lightColors[i % _lightColors.size()];
                _danceLights[i].ShadowPercent = 0.0f;
            }

            for (uint32_t i = 0; i < _fighters.size(); i++)
            {
                _fighterActionTimers[i] -= dt;
                if (_fighterActionTimers[i] < 0.0f)
                {
                    if (_firstUpdate || _fighters[i]->GetCurrentState() == Character::CharacterState_Idle)
                    {
                        Character::Character* closestDummy = nullptr;
                        float closestDummyDist = std::numeric_limits<float>::max();
                        for (Character::Character* dummy : _dummies)
                        {
                            float distToDummy = Vector2f::Distance(_fighters[i]->GetPosition(), dummy->GetPosition());
                            if (distToDummy < closestDummyDist && _fighters[i]->CanMoveTo(dummy->GetPosition(), 100.0f))
                            {
                                closestDummy = dummy;
                                closestDummyDist = distToDummy;
                            }
                        }

                        if (closestDummy)
                        {
                            _fighters[i]->PushAction(Character::CreateAttackAction(closestDummy->GetID()), false);
                        }
                        else
                        {
                            _fighters[i]->PushAction(Character::CreateEmptyAction(), false);
                        }

                        if (_firstUpdate)
                        {
                            _fighterActionTimers[i] = Random::RandomBetween(0.0f, 8.0f);
                        }
                        else
                        {
                            _fighterActionTimers[i] = Random::RandomBetween(5.0f, 8.0f);
                        }
                    }
                    else
                    {
                        _fighters[i]->PushAction(Character::CreateEmptyAction(), false);
                        _fighterActionTimers[i] = Random::RandomBetween(8.0f, 15.0f);
                    }
                }
            }

            _firstUpdate = false;
        }

        void MenuLevel::OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            BasicLevel::OnDraw(layer, levelRenderer);

            if (layer->IsPrimary())
            {
                for (uint32_t i = 0; i < _danceLights.size(); i++)
                {
                    levelRenderer->AddLight(_danceLights[i]);
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Level::MenuLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);

        EnumeratePreloads<Character::Dummy>(preloads);
        EnumeratePreloads<Character::MinerDwarf>(preloads);
        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Character::BrewerDwarf>(preloads);

        EnumeratePreloads<Character::MrBonesMelee>(preloads);
        EnumeratePreloads<Character::Gobbo>(preloads);

        EnumeratePreloads<Character::MenuController>(preloads);
        EnumeratePreloads<Character::Torch>(preloads);
        EnumeratePreloads<Character::WallBrazier>(preloads);
        EnumeratePreloads<Character::PotBrazier>(preloads);
        EnumeratePreloads<Character::HangingBrazier>(preloads);

        EnumeratePreloads<Item::WoodPickAxe>(preloads);
        EnumeratePreloads<Item::IronPickAxe>(preloads);
        EnumeratePreloads<Item::GoldPickAxe>(preloads);
        EnumeratePreloads<Item::WoodenShortSword>(preloads);
        EnumeratePreloads<Item::WoodenBasicShield>(preloads);
        EnumeratePreloads<Item::BasicRifle>(preloads);

        EnumeratePreloads<Character::ResourceNode>(preloads);
        preloads.insert("Skeletons/Characters/skellynode/skellynode.skel");
        preloads.insert("Skeletons/Characters/skellynode/skellynode.polymatset");

        EnumerateMusicSwellMapPreloads(preloads, Level::MenuMusicSwells);
    }
}
