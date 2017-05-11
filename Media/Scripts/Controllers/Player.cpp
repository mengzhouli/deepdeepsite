#include "Controllers/Player.hpp"

#include "HUD/PanelBorder.hpp"
#include "Application/CursorSet.hpp"

#include "Settings/TheDeepDeepProfile.hpp"

#include "Items/GameItems.hpp"
#include "Abilities/GameAbilities.hpp"
#include "Characters/GameCharacters.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"

#include "NavigationUtility.hpp"

#include "HUD/Minimap.hpp"
#include "HUD/SelectionArea.hpp"
#include "HUD/CharacterSelector.hpp"
#include "HUD/MenuDisplay.hpp"
#include "HUD/ActionDisplay.hpp"
#include "HUD/PauseButton.hpp"
#include "HUD/ResourceDisplay.hpp"
#include "HUD/Debugger.hpp"
#include "HUD/DebuggerElements.hpp"
#include "HUD/TutorialDisplay.hpp"

#include "Levels/CampaignLevel.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const float MaxMoveSearchDist = 800.0f;
        static const Color MinCharacterColor = Color::FromFloats(0.2f, 0.2f, 0.2f, 1.0f);

        static const Color SelectedCharacterHighlightColor = Color::FromFloats(0.75f, 0.75f, 0.75f, 1.0f);
        static const Color InteractiveCharacterHighlightColor = Color::FromFloats(0.75f, 0.75f, 0.1f, 1.0f);
        static const Color DestructableCharacterHighlightColor = Color::FromFloats(0.75f, 0.4f, 0.1f, 1.0f);
        static const Color AttackableCharacterHighlightColor = Color::FromFloats(0.75f, 0.1f, 0.1f, 1.0f);
        static const Color InteractiveItemHighlightColor = Color::FromFloats(0.75f, 0.75f, 0.1f, 1.0f);

        static const std::pair<float, float> AffirmitiveSoundResetRange = { 1.0f, 2.0f };
        static const std::pair<float, float> NegatorySoundResetRange = { 0.0f, 0.0f };

        static Rectangleu GetHUDClippedViewport(const Rectangleu& viewport)
        {
            const float maxUIAspectRatio = 16.0f / 9.0f;

            Rectangleu clippedViewport = viewport;
            if ((viewport.W / static_cast<float>(viewport.H)) > maxUIAspectRatio)
            {
                uint32_t newWidth = viewport.H * maxUIAspectRatio;
                uint32_t widthDiff = viewport.W - newWidth;
                clippedViewport.X += (widthDiff / 2);
                clippedViewport.W -= widthDiff;
            }
            return clippedViewport;
        }

        Player::Player(const ControllerParameters& parameters)
            : Controller(parameters)
        {
            _prevViewport = Rectangleu(0, 0, 0, 0);
            _uiScale = 0.0f;

            _minScrollDist = 10.0f;
            _camMoveSpeed = 1000.0f;

            _minZoom = 0.1f;
            _maxZoom = 5.0f;
            _zoomSpeed = 0.1f;

            _mouseLightOn = false;

            _curContext = Context_None;

            const Settings::Profile* profile = GetLevel()->GetProfile();

            _availableCharacterBinds =
            {
                profile->GetBindCode("character_0"),
                profile->GetBindCode("character_1"),
                profile->GetBindCode("character_2"),
                profile->GetBindCode("character_3"),
                profile->GetBindCode("character_4"),
                profile->GetBindCode("character_5"),
                profile->GetBindCode("character_6"),
                profile->GetBindCode("character_7"),
                profile->GetBindCode("character_8"),
                profile->GetBindCode("character_9"),
                profile->GetBindCode("character_10"),
                profile->GetBindCode("character_11"),
                profile->GetBindCode("character_12"),
            };

            _hudSelectBind = profile->GetBindCode("hud_select");

            _moveUpBind = profile->GetBindCode("up");
            _moveDownBind = profile->GetBindCode("down");
            _moveLeftBind = profile->GetBindCode("left");
            _moveRightBind = profile->GetBindCode("right");

            _pauseBind = profile->GetBindCode("pause");

            _showDebugUIBind = profile->GetBindCode("debug_ui");
            _debugBinds =
            {
                profile->GetBindCode("debug_1"),
                profile->GetBindCode("debug_2"),
                profile->GetBindCode("debug_3"),
                profile->GetBindCode("debug_4"),
                profile->GetBindCode("debug_5"),
                profile->GetBindCode("debug_6"),
                profile->GetBindCode("debug_7"),
                profile->GetBindCode("debug_8"),
                profile->GetBindCode("debug_9"),
                profile->GetBindCode("debug_0"),
            };

            _queueBind = profile->GetBindCode("queue");
            _cancelBind = profile->GetBindCode("cancel");
            _holdPositionBind = profile->GetBindCode("hold_position");

            _selectAllBind = profile->GetBindCode("select_all");
            _iterateSelectionBind = profile->GetBindCode("iterate_selection");

            _selectBind = profile->GetBindCode("select");
            _actionBind = profile->GetBindCode("action");

            _attackBind = profile->GetBindCode("attack");
            _moveBind = profile->GetBindCode("move");
            _stopBind = profile->GetBindCode("stop");
            _holdBind = profile->GetBindCode("hold_position");
            _abilityBinds =
            {
                profile->GetBindCode("ability_0"),
                profile->GetBindCode("ability_1"),
                profile->GetBindCode("ability_2"),
                profile->GetBindCode("ability_3"),
            };

            _selectedCharacterTypeIdx = 0;
            _selectedCharacterIdx = 0;

            _activeAbility = nullptr;
            _setFirstAbilityPosition = false;

            _minimap = nullptr;
            _hudSelectionArea = nullptr;
            _hudCharacterSelector = nullptr;
            _hudActionDisplay = nullptr;
            _hudMenuDisplay = nullptr;

            _debugger = nullptr;

            _inCutscene = false;
            _onVictoryScreen = false;
            _onDefeatScreen = false;
            _paused = false;
            _hudVisible = false;

            _interactiveSelectionCursor = nullptr;
            _tooltip = nullptr;

            _firstUpdate = true;
        }

        void Player::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) const
        {
            std::vector<Dwarf*> dwarves = GetCharacters<Dwarf>();
            for (const auto& dwarf : dwarves)
            {
                if (profile->HasCharacterInfo(dwarf->GetName()))
                {
                    profile->UpdateCharacterInfo(dwarf->GetName(), dwarf->GetInfo());
                }
            }

            for (auto shownTutorial : _tutorialDisplay->GetShownTutorials())
            {
                profile->MarkTutorialShown(shownTutorial);
            }
        }

        void Player::OnCreate()
        {
        }

        void Player::OnDestroy()
        {
            SafeRelease(_tooltip);
            SafeRelease(_interactiveSelectionCursor);
        }

        void Player::OnStartCutscene()
        {
            if (!_firstUpdate)
            {
                animateHUDOut(false);
            }

            _inCutscene = true;
            setContext(Context_None);
            SafeRelease(_tooltip);
            SafeRelease(_interactiveSelectionCursor);
        }

        void Player::OnEndCutscene()
        {
            if (!_firstUpdate)
            {
                animateHUDIn(false);
            }

            _inCutscene = false;
        }

        void Player::OnAddCharacter(Character* character)
        {
            // Skip constructed characters
            if ((character->GetEntityMask() & CharacterMask_Constructed) != 0)
            {
                return;
            }

            _allCharacters.AddCharacter(character);

            _hudCharacterSelector->SetCharacters(_allCharacters);

            // Don't give characters binds before the first update
            if (!_firstUpdate)
            {
                // Give this character a bind if one is available
                if (!_availableCharacterBinds.empty())
                {
                    character->SetBind(_availableCharacterBinds.front());
                    _availableCharacterBinds.pop_front();
                }
                else
                {
                    character->SetBind(Input::InputBindCode_Invalid);
                }
            }

            if (IsA<SkeletonCharacter>(character))
            {
                AsA<SkeletonCharacter>(character)->SetMinimumColor(MinCharacterColor);
            }
        }

        void Player::OnRemoveCharacter(Character* character)
        {
            Character* selectedCharacter = _selectedCharacters.GetCharacter(_selectedCharacterTypeIdx, _selectedCharacterIdx);

            _allCharacters.RemoveCharacter(character);
            _selectedCharacters.RemoveCharacter(character);

            if (character == selectedCharacter)
            {
                iterateSelectedCharacter();
            }

            if (_hudCharacterSelector)
            {
                _hudCharacterSelector->SetCharacters(_allCharacters);
            }
            if (_hudSelectionArea)
            {
                _hudSelectionArea->SetSelectedCharacter(_selectedCharacters.GetCharacter(_selectedCharacterTypeIdx, _selectedCharacterIdx));
            }

            // Reclaim the bind
            Input::InputBindCode bind = character->GetBind();
            if (bind != Input::InputBindCode_Invalid)
            {
                _availableCharacterBinds.push_front(bind);
                character->SetBind(Input::InputBindCode_Invalid);
            }
        }

        void Player::OnLoadContent(Content::ContentManager* contentManager)
        {
            Level::LevelInstance* lvl = GetLevel();
            const Localization::StringTable* strings = lvl->GetStringTable();

            const App::CursorSet* cursorSet = contentManager->Load<App::CursorSet>("HUD/hud.cursorset");

            _defaultCursor = cursorSet->GetCursor("cursor_regular");
            SafeAddRef(_defaultCursor);

            _cursors[Context_None] = cursorSet->GetCursor("cursor_regular");
            _cursors[Context_Attack] = cursorSet->GetCursor("cursor_attack");
            _cursors[Context_Move] = cursorSet->GetCursor("cursor_regular");
            _cursors[Context_Ability] = cursorSet->GetCursor("cursor_regular");
            _cursors[Context_Selecting] = cursorSet->GetCursor("cursor_regular");

            for (auto cursor : _cursors)
            {
                SafeAddRef(cursor.second);
            }

            _defaultInteractCursor = cursorSet->GetCursor("cursor_interact");
            SafeAddRef(_defaultInteractCursor);

            SafeRelease(cursorSet);

            lvl->SetCharacterPortraitEnabled(true);
            _hudSelectionArea = new HUD::SelectionArea(contentManager, strings, lvl->GetProfile(), lvl->GetCharacterPortrait(), _hudSelectBind, HUD::SelectionAreaType::Small);

            lvl->SetMinimapEnabled(true);
            _minimap = new HUD::Minimap(lvl->GetMinimap(), contentManager, _hudSelectBind);

            _hudCharacterSelector = new HUD::CharacterSelector(contentManager, _hudSelectBind, _selectAllBind, HUD::CharacterStackLocation::Center);
            _hudActionDisplay = new HUD::ActionDisplay(contentManager, lvl->GetPrimaryLayer());
            _hudMenuDisplay = new HUD::MenuDisplay(contentManager, strings, lvl->GetProfile(), lvl->GetSoundManager(), _hudSelectBind);
            _hudPauseButton = new HUD::PauseButton(contentManager, strings, _hudSelectBind);
            _hudResourceDisplay = new HUD::ResourceDisplay(contentManager, strings, _hudSelectBind);
            _tutorialDisplay = new HUD::TutorialDisplay(contentManager, strings, lvl->GetSoundManager(), _hudSelectBind, lvl->GetProfile<Settings::TheDeepDeepProfile>());

            _debugger = new HUD::Debugger(lvl->GetProfile(), lvl->GetStringTable(), _debugBinds);

            {
                {
                    auto mouseLightDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Mouse light", [this]() {return _mouseLightOn; }, [this](bool value) { _mouseLightOn = value; });
                    _debugger->AddElement("General", "Mouse light", mouseLightDebugElement);
                }

                {
                    auto showHudDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Show HUD", [this]() { return _hudVisible; },
                            [this](bool value) {
                        if (value != _hudVisible)
                        {
                            if (value)
                            {
                                animateHUDIn(true);
                            }
                            else
                            {
                                animateHUDOut(true);
                            }
                        }
                    });
                    _debugger->AddElement("General", "Show HUD", showHudDebugElement);
                }

                {
                    auto timeDialationSlider =
                        std::make_shared<HUD::FloatSliderDebuggerElement>("Time Dialation", 0.0f, 2.0f, [this]() {return GetLevel()->GetTimeDialation(); }, [this](float value) { GetLevel()->SetTimeDialation(value); });
                    _debugger->AddElement("General", "Time Dialation", timeDialationSlider);
                }

            }

            {
                auto soundManager = GetLevel()->GetSoundManager();

                {
                    auto showEnvironementsDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Show Environments",
                            [soundManager]() { return soundManager->IsDrawingEnvironments(); },
                            [soundManager](bool value) { soundManager->SetDrawEnvironments(value); });
                    _debugger->AddElement("Audio", "Show Environments", showEnvironementsDebugElement);
                }

                {
                    auto showSoundsDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Show Sounds",
                            [soundManager]() { return soundManager->IsDrawingSounds(); },
                            [soundManager](bool value) { soundManager->SetDrawSounds(value); });
                    _debugger->AddElement("Audio", "Show Sounds", showSoundsDebugElement);
                }

                {
                    auto defaultEnvironmentTypeDebugElement =
                        std::make_shared<HUD::ComboboxDebuggerElement>("Default environment", false,
                    [soundManager]()
                    {
                        std::vector<std::string> values;
                        int curValue = -1;

                        for (auto environmentType : Audio::EnvironmentType::_values())
                        {
                            values.push_back(environmentType._to_string());
                            if (environmentType == soundManager->GetDefaultEnvironmentType())
                            {
                                curValue = values.size() - 1;
                            }
                        }

                        return std::make_pair(values, curValue);
                    },
                    [soundManager](int32_t newValue)
                    {
                        int32_t idx = 0;
                        for (auto environmentType : Audio::EnvironmentType::_values())
                        {
                            if (idx == newValue)
                            {
                                soundManager->SetDefaultEnvironmentType(environmentType);
                                return;
                            }

                            idx++;
                        }
                    });
                    _debugger->AddElement("Audio", "Default environment", defaultEnvironmentTypeDebugElement);
                }

                for (auto environmentType : Audio::EnvironmentType::_values())
                {
                    auto reverb = soundManager->GetEvironmentReverbEffect(environmentType);
                    if (reverb == nullptr)
                    {
                        continue;
                    }

                    std::string name = Format("Environment: %s", environmentType._to_string());
                    auto audioEnvironmentDebugElement =
                        std::make_shared<HUD::ReverbEffectDebuggerElement>(name, false, reverb);
                    _debugger->AddElement("Audio", name, audioEnvironmentDebugElement);
                }
            }

            {
                auto getPathSystem = [this]()
                {
                    return GetLevel()->GetPrimaryLayer()->GetPathSystem();
                };

                {
                    auto drawPathDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw path nodes",
                            [getPathSystem]() { return getPathSystem()->GetDrawPathNodes(); },
                            [getPathSystem](bool value) { getPathSystem()->SetDrawPathNodes(value); });
                    _debugger->AddElement("Pathfinding", "Draw path nodes", drawPathDebugElement);
                }
                {
                    auto drawPathDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw path lines",
                            [getPathSystem]() { return getPathSystem()->GetDrawPathLines(); },
                            [getPathSystem](bool value) { getPathSystem()->SetDrawPathLines(value); });
                    _debugger->AddElement("Pathfinding", "Draw path lines", drawPathDebugElement);
                }
                {
                    auto drawPathDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw path normals",
                            [getPathSystem]() { return getPathSystem()->GetDrawPathNormals(); },
                            [getPathSystem](bool value) { getPathSystem()->SetDrawPathNormals(value); });
                    _debugger->AddElement("Pathfinding", "Draw path normals", drawPathDebugElement);
                }
                {
                    auto drawPathDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw active paths",
                            [getPathSystem]() { return getPathSystem()->GetDrawActivePaths(); },
                            [getPathSystem](bool value) { getPathSystem()->SetDrawActivePaths(value); });
                    _debugger->AddElement("Pathfinding", "Draw active paths", drawPathDebugElement);
                }
                {
                    auto drawPathDebugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw active path positions",
                            [getPathSystem]() { return getPathSystem()->GetDrawPathPositions(); },
                            [getPathSystem](bool value) { getPathSystem()->SetDrawActivePositions(value); });
                    _debugger->AddElement("Pathfinding", "Draw active path positions", drawPathDebugElement);
                }
            }

            {
                auto getPhysics = [this]()
                {
                    return GetLevel()->GetPrimaryLayer()->GetPhysicsWorld();
                };

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw shapes",
                            [getPhysics]() { return getPhysics()->GetDrawShapes(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawShapes(value); });
                    _debugger->AddElement("Physics", "Draw shapes", debugElement);
                }

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw joints",
                            [getPhysics]() { return getPhysics()->GetDrawJoints(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawJoints(value); });
                    _debugger->AddElement("Physics", "Draw joints", debugElement);
                }

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw AABBs",
                            [getPhysics]() { return getPhysics()->GetDrawAABBs(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawAABBs(value); });
                    _debugger->AddElement("Physics", "Draw AABBs", debugElement);
                }

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw pairs",
                            [getPhysics]() { return getPhysics()->GetDrawPairs(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawPairs(value); });
                    _debugger->AddElement("Physics", "Draw pairs", debugElement);
                }

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw center of mass",
                            [getPhysics]() { return getPhysics()->GetDrawCenterOfMasses(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawCenterOfMasses(value); });
                    _debugger->AddElement("Physics", "Draw center of mass", debugElement);
                }

                {
                    auto debugElement =
                        std::make_shared<HUD::CheckboxDebuggerElement>("Draw active percent",
                            [getPhysics]() { return getPhysics()->GetDrawActivePerc(); },
                            [getPhysics](bool value) { getPhysics()->SetDrawActivePerc(value); });
                    _debugger->AddElement("Physics", "Draw active percent", debugElement);
                }

                {
                    auto spawnCharacterElement = HUD::CreateStandardCharacterSpawnerElement(contentManager, lvl->GetPrimaryLayer(), [this]() { return _mousePosWorld; });
                    _debugger->AddBindableElement("Spawn Character", spawnCharacterElement);
                }

                {
                    auto impulseElement = HUD::CreateImpulseElement(lvl->GetPrimaryLayer(), [this]() { return _mousePosWorld; });
                    _debugger->AddBindableElement("Create Impulse", impulseElement);
                }

                {
                    auto soundElement = HUD::CreatePlaySoundElement(lvl->GetPrimaryLayer(), [this]() { return _mousePosWorld; }, contentManager);
                    _debugger->AddBindableElement("Play Sound", soundElement);
                }
            }

            Level::BasicLevel* levelAsBasic = AsA<Level::BasicLevel>(lvl);
            if (levelAsBasic)
            {
                levelAsBasic->InitializeDebugger(_debugger);
            }
        }

        void Player::OnUnloadContent()
        {
            SafeRelease(_defaultCursor);
            for (auto cursor : _cursors)
            {
                SafeRelease(cursor.second);
            }
            _cursors.clear();
            SafeRelease(_defaultInteractCursor);

            SafeDelete(_hudSelectionArea);
            SafeDelete(_minimap);
            SafeDelete(_hudCharacterSelector);
            SafeRelease(_hudActionDisplay);
            SafeDelete(_hudMenuDisplay);
            SafeDelete(_hudPauseButton);
            SafeDelete(_hudResourceDisplay);
            SafeDelete(_debugger);
            SafeDelete(_tutorialDisplay);
        }

        void Player::OnUpdate(const Input::FrameInput& input, double totalTime, float dt)
        {
            if (_firstUpdate)
            {
                if (!_inCutscene)
                {
                    animateHUDIn(false);
                }

                // Assign initial bindings to the characters
                for (int32_t i = int32_t(_allCharacters.Count()) - 1; i >= 0; i--)
                {
                    Character* character = _allCharacters.GetCharacter(i);
                    assert(character->GetBind() == Input::InputBindCode_Invalid);

                    // Give this character a bind if one is available
                    if (!_availableCharacterBinds.empty())
                    {
                        character->SetBind(_availableCharacterBinds.front());
                        _availableCharacterBinds.pop_front();
                    }
                    else
                    {
                        character->SetBind(Input::InputBindCode_Invalid);
                    }
                }

                _selectedCharacters = _allCharacters;

                _firstUpdate = false;
            }

            SafeRelease(_tooltip);
            SafeRelease(_interactiveSelectionCursor);

            Level::LevelInstance* lvl = GetLevel();
            Level::LevelLayerInstance* primaryLayer = lvl->GetPrimaryLayer();
            Level::CameraController& cameraController = lvl->GetCameraController();
            const Camera& camera = cameraController.GetCamera();

            const Camera& hudCamera = lvl->GetHUDCamera();
            const Rectanglef& viewport = hudCamera.GetViewport();

            Vector2f mousePosHUD = input.UnProjectMousePosition(hudCamera);
            Vector2f mousePosWorld = input.UnProjectMousePosition(camera);
            _mousePosScreen = mousePosHUD;
            _mousePosWorld = mousePosWorld;

            const Settings::GameProfile* profile = GetLevel()->GetProfile<Settings::GameProfile>();
            float uiScale = profile->GetUIScale();

            if (Rectangleu(viewport) != _prevViewport || uiScale != _uiScale)
            {
                PositionHUDButtons(viewport, uiScale);
                _prevViewport = viewport;
                _uiScale = uiScale;
            }

            // Unset character highlights
            for (auto highlightCharacter : _highlightCharacters)
            {
                auto character = primaryLayer->GetCharacter(highlightCharacter.first);
                if (character)
                {
                    character->SetHighlightColor(Color::Transparent);
                }
            }
            _highlightCharacters.clear();

            for (auto highlightItem : _highlightItems)
            {
                auto item = primaryLayer->GetItem(highlightItem.first);
                if (item != nullptr)
                {
                    item->SetHighlightColor(Color::Transparent);
                }
            }
            _highlightItems.clear();

            bool mouseOverUI = false;

            if (input.IsBindJustPressed(_showDebugUIBind))
            {
                _debugger->SetVisible(!_debugger->IsVisible());
            }

            _debugger->Update(totalTime, dt, input);
            mouseOverUI = mouseOverUI || _debugger->IsMouseOver();

            _hudPauseButton->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _hudPauseButton->IsMouseOver();

            _hudResourceDisplay->SetResources(GetResources());
            _hudResourceDisplay->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _hudResourceDisplay->IsMouseOver();

            _hudCharacterSelector->Update(totalTime, dt, input, hudCamera, _selectedCharacters);
            mouseOverUI = mouseOverUI || _hudCharacterSelector->IsMouseOver();

            _tutorialDisplay->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _tutorialDisplay->IsMouseOver();

            if ((input.IsBindJustPressed(_pauseBind) || _hudPauseButton->IsJustClicked()) && !_onVictoryScreen && !_onDefeatScreen)
            {
                if (_paused)
                {
                    unpause();
                }
                else
                {
                    pause();
                }
            }

            _hudMenuDisplay->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _hudMenuDisplay->IsMouseOver();

            if (_paused)
            {
                Level::CampaignLevel* level = AsA<Level::CampaignLevel>(GetLevel());
                if (_onVictoryScreen)
                {
                    if (_hudMenuDisplay->IsForwardButtonJustPressed() && level != nullptr)
                    {
                        // Tell level to go back to campaign menu
                        level->TransitionToCampaignMenu();
                    }
                    else if (_hudMenuDisplay->IsBackButtonJustPressed())
                    {
                        // Tell level to ???
                    }
                }
                else if (_onDefeatScreen)
                {
                    uint32_t selectedCheckpoint;
                    if (_hudMenuDisplay->IsForwardButtonJustPressed())
                    {
                        // Tell level to ???
                    }
                    else if (_hudMenuDisplay->IsBackButtonJustPressed() && level != nullptr)
                    {
                        // Tell level to go back to campaign menu
                        level->TransitionToCampaignMenu();
                    }
                    else if (_hudMenuDisplay->IsRestartAtLoadoutButtonPressed())
                    {
                        // Tell level to go back to the loadout menu
                        level->RestartAtLoadout();
                    }
                    else if (_hudMenuDisplay->IsCheckpointButtonPressed(selectedCheckpoint) && level != nullptr)
                    {
                        // Tell level to go to checkpoint
                        level->RestartAtCheckpoint(selectedCheckpoint);
                    }
                }
                else
                {
                    // Regular pause screen

                    uint32_t selectedCheckpoint;
                    if (_hudMenuDisplay->IsForwardButtonJustPressed())
                    {
                        // Resume
                        unpause();
                    }
                    else if (_hudMenuDisplay->IsBackButtonJustPressed() && level != nullptr)
                    {
                        // Tell level to go back to campaign menu
                        level->TransitionToCampaignMenu();
                    }
                    else if (_hudMenuDisplay->IsRestartAtLoadoutButtonPressed())
                    {
                        // Tell level to go back to the loadout menu
                        level->RestartAtLoadout();
                    }
                    else if (_hudMenuDisplay->IsCheckpointButtonPressed(selectedCheckpoint) && level != nullptr)
                    {
                        // Tell level to go to checkpoint
                        level->RestartAtCheckpoint(selectedCheckpoint);
                    }
                }
            }

            if (!_paused && !_inCutscene)
            {
                Character* hudSelectorClickedCharacter = _hudCharacterSelector->GetClickedCharacter();
                if (hudSelectorClickedCharacter)
                {
                    if (input.IsBindPressed(_queueBind))
                    {
                        if (_selectedCharacters.ContainsCharacter(hudSelectorClickedCharacter))
                        {
                            _selectedCharacters.RemoveCharacter(hudSelectorClickedCharacter);
                        }
                        else
                        {
                            _selectedCharacters.AddCharacter(hudSelectorClickedCharacter);
                        }
                    }
                    else
                    {
                        clearSelection();
                        _selectedCharacters.AddCharacter(hudSelectorClickedCharacter);
                    }
                    _selectionChanged = true;
                }
                else if (input.IsBindJustPressed(_selectAllBind))
                {
                    for (uint32_t i = 0; i < _allCharacters.Count(); i++)
                    {
                        Character* character = _allCharacters.GetCharacter(i);
                        if (!_selectedCharacters.ContainsCharacter(character))
                        {
                            _selectedCharacters.AddCharacter(character);
                        }
                    }
                    _selectionChanged = true;
                }

                for (uint32_t i = 0; i < _allCharacters.Count(); i++)
                {
                    Character* character = _allCharacters.GetCharacter(i);
                    if (input.IsBindJustPressed(character->GetBind()))
                    {
                        if (input.IsBindPressed(_queueBind))
                        {
                            if (_selectedCharacters.ContainsCharacter(character))
                            {
                                _selectedCharacters.RemoveCharacter(character);
                            }
                            else
                            {
                                _selectedCharacters.AddCharacter(character);
                            }
                        }
                        else
                        {
                            clearSelection();
                            _selectedCharacters.AddCharacter(character);
                        }

                        _selectionChanged = true;
                    }
                    if (input.IsBindJustDoublePressed(character->GetBind()))
                    {
                        cameraController.Pan(character->GetBounds().Middle(), 0.0f);
                    }
                }

                if (input.IsBindJustPressed(_iterateSelectionBind))
                {
                    iterateSelectedCharacter();
                }
            }

            Character* selectedCharacter = _selectedCharacters.GetCharacter(_selectedCharacterTypeIdx, _selectedCharacterIdx);

            _hudSelectionArea->SetSelectedCharacter(selectedCharacter);
            _hudSelectionArea->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _hudSelectionArea->IsMouseOver();

            _minimap->Update(totalTime, dt, input, hudCamera);
            mouseOverUI = mouseOverUI || _minimap->IsMouseOver();

            if (!_paused && !_inCutscene)
            {
                // Camera movement
                panCamera(mousePosHUD, viewport, input, dt);

                Vector2f minimapClickPosition;
                if (_minimap->IsClicked(minimapClickPosition))
                {
                    cameraController.Pan(minimapClickPosition);
                }

                if (_hudSelectionArea->IsPortraitClicked())
                {
                    cameraController.Pan(selectedCharacter->GetBounds().Middle());
                }

                // Context switching
                bool contextJustChanged = false;
                if (_curContext == Context_None && input.IsBindJustPressed(_selectBind) && !mouseOverUI)
                {
                    _selectionOrigin = mousePosWorld;
                    _selectionExtent = mousePosWorld;
                    setContext(Context_Selecting);
                    contextJustChanged = true;
                }
                else if (input.IsBindJustPressed(_attackBind) || _hudSelectionArea->IsAttackButtonJustClicked())
                {
                    setContext(Context_Attack);
                    contextJustChanged = true;
                }
                else if (input.IsBindJustPressed(_moveBind) || _hudSelectionArea->IsMoveButtonJustClicked())
                {
                    setContext(Context_Move);
                    contextJustChanged = true;
                }
                else if (selectedCharacter)
                {
                    uint32_t clickedAbilityIdx;
                    bool abilityClicked = _hudSelectionArea->IsAbilityButtonJustClicked(clickedAbilityIdx);

                    for (uint32_t i = 0; i < selectedCharacter->GetAbilityCount(); i++)
                    {
                        if (input.IsBindJustPressed(_abilityBinds[i]) || (abilityClicked && i == clickedAbilityIdx))
                        {
                            // Set ability and current ability state
                            setContext(Context_Ability);
                            contextJustChanged = true;

                            _activeAbility = selectedCharacter->GetAbility(i);
                            _activeAbility->ClearState();
                            _setFirstAbilityPosition = false;

                            break;
                        }
                    }
                }

                // Context update
                if (!contextJustChanged)
                {
                    if (_curContext == Context_None)
                    {
                        if (input.IsBindJustPressed(_actionBind))
                        {
                            // Attempt to determine the best action
                            SelectedCharactersDoAction(mousePosWorld, input.IsBindPressed(_queueBind));
                            setContext(Context_None);
                        }

                        if (input.IsBindJustPressed(_stopBind) || _hudSelectionArea->IsStopButtonJustClicked())
                        {
                            for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
                            {
                                Character* character = _selectedCharacters.GetCharacter(i);
                                character->ClearActions();
                            }
                        }

                        if (input.IsBindJustPressed(_holdPositionBind) || _hudSelectionArea->IsHoldPositionButtonJustClicked())
                        {
                            for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
                            {
                                Character* character = _selectedCharacters.GetCharacter(i);
                                character->PushAction(CreateHoldPositionAction(), input.IsBindPressed(_queueBind));
                            }
                        }

                        if (input.IsBindJustPressed(_cancelBind))
                        {
                            clearSelection();
                        }
                    }
                    else if (_curContext == Context_Attack)
                    {
                        if (input.IsBindJustPressed(_cancelBind) || input.IsBindJustPressed(_actionBind))
                        {
                            setContext(Context_None);
                        }
                        else if (input.IsBindJustPressed(_actionBind) || input.IsBindJustPressed(_selectBind))
                        {
                            if (!mouseOverUI)
                            {
                                SelectedCharactersAttack(mousePosWorld, true, input.IsBindPressed(_queueBind));
                            }
                            setContext(Context_None);
                        }
                    }
                    else if (_curContext == Context_Move)
                    {
                        if (input.IsBindJustPressed(_cancelBind) || input.IsBindJustPressed(_actionBind))
                        {
                            setContext(Context_None);
                        }
                        else if (input.IsBindJustPressed(_actionBind) || input.IsBindJustPressed(_selectBind))
                        {
                            if (!mouseOverUI)
                            {
                                SelectedCharactersMove(mousePosWorld, input.IsBindPressed(_queueBind));
                            }
                            setContext(Context_None);
                        }
                    }
                    else if (_curContext == Context_Selecting)
                    {
                        _selectionExtent = mousePosWorld;

                        if (input.IsBindJustPressed(_cancelBind) || input.IsBindJustPressed(_actionBind))
                        {
                            setContext(Context_None);
                        }
                        else if (input.IsBindJustReleased(_selectBind))
                        {
                            bool clearedSelection = false;

                            Rectanglef selectionArea = getSelectionRectangle();
                            for (uint32_t i = 0; i < _allCharacters.Count(); i++)
                            {
                                // Iterate backwards so that the topmost character is selected if only one ends up getting selected
                                Character* character = _allCharacters.GetCharacter((_allCharacters.Count() - 1) - i);

                                Vector2f outHitPos;
                                if (character->Intersects(selectionArea, outHitPos))
                                {
                                    if (!clearedSelection && !input.IsBindPressed(_queueBind))
                                    {
                                        clearSelection();
                                        clearedSelection = true;
                                    }

                                    // Depending on whether this character is already selected, add or remove
                                    // it from the selection
                                    if (_selectedCharacters.ContainsCharacter(character))
                                    {
                                        _selectedCharacters.RemoveCharacter(character);
                                    }
                                    else
                                    {
                                        _selectedCharacters.AddCharacter(character);
                                    }

                                    _selectionChanged = true;

                                    // Only select one character if the selection area is small (likely just a click instead of a drag)
                                    if (selectionArea.W <= 1 && selectionArea.H <= 1)
                                    {
                                        break;
                                    }
                                }
                            }

                            if (_selectedCharacters.Count() > 0)
                            {
                                LogInfo(Format("Selected %u characters.", _selectedCharacters.Count()));
                            }

                            setContext(Context_None);
                        }
                    }
                    else if (_curContext == Context_Ability)
                    {
                        if (_activeAbility->IsOnCooldown())
                        {
                            setContext(Context_None);
                        }
                        else
                        {
                            if (input.IsBindJustPressed(_cancelBind) || input.IsBindJustPressed(_actionBind))
                            {
                                setContext(Context_None);
                            }
                            else
                            {
                                Ability::AbilityType activeAbilityType = _activeAbility->GetAbilityType();

                                auto generateAbilityAction = [this, mouseOverUI, input]()
                                {
                                    if (_activeAbility->IsInValidState() &&
                                        (_activeAbility->GetAbilityType() == Ability::AbilityType_NoTarget || !mouseOverUI))
                                    {
                                        auto owner = _activeAbility->GetOwner();
                                        auto action = _activeAbility->GenerateAction();
                                        bool queue = input.IsBindPressed(_queueBind);
                                        if (_activeAbility->IsInstant() && !queue)
                                        {
                                            owner->PushActionFront(action);
                                        }
                                        else
                                        {
                                            owner->PushAction(action, queue);
                                        }

                                        _abilityUsed = true;
                                    }
                                    else
                                    {
                                        PlaySelectedCharacterNegatory();
                                    }

                                    _activeAbility->ClearState();
                                };

                                if (activeAbilityType == Ability::AbilityType_NoTarget)
                                {
                                    generateAbilityAction();
                                    setContext(Context_None);
                                }
                                else if (activeAbilityType == Ability::AbilityType_OneTarget)
                                {
                                    _activeAbility->SetPrimaryTarget(mousePosWorld);
                                    if (input.IsBindJustPressed(_selectBind))
                                    {
                                        generateAbilityAction();
                                        setContext(Context_None);
                                    }
                                }
                                else if (activeAbilityType == Ability::AbilityType_TwoTargets)
                                {
                                    if (!_setFirstAbilityPosition)
                                    {
                                        _activeAbility->SetPrimaryTarget(mousePosWorld);
                                    }
                                    else
                                    {
                                        _activeAbility->SetSecondaryTarget(mousePosWorld);
                                    }

                                    if (input.IsBindJustPressed(_selectBind))
                                    {
                                        if (_setFirstAbilityPosition)
                                        {
                                            generateAbilityAction();
                                            setContext(Context_None);
                                        }
                                        else
                                        {
                                            _setFirstAbilityPosition = true;
                                        }
                                    }
                                }
                                else
                                {
                                    setContext(Context_None);
                                }
                            }
                        }
                    }
                }

                // Find characters to add highlights to
                for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
                {
                    _highlightCharacters[_selectedCharacters.GetCharacter(i)->GetID()] = SelectedCharacterHighlightColor;
                }

                // Look for something to use as a tooltip or interact cursor
                if (!mouseOverUI)
                {
                    bool foundHighlight = false;

                    if (!foundHighlight)
                    {
                        std::vector<Item::Item*> itemsUnderMouse = primaryLayer->FindItems<Item::Item>(
                            [&](const Item::Item* item)
                            {
                                if (!item->IsInteractive())
                                {
                                    return false;
                                }

                                Physics::Collision* collision = item->GetCollision();
                                if (collision == nullptr || !collision->Contains(mousePosWorld))
                                {
                                    return false;
                                }

                                if (!isItemHighlightable(item))
                                {
                                    return false;
                                }

                                return true;
                            });
                        if (!itemsUnderMouse.empty())
                        {
                            Item::Item* item = itemsUnderMouse.back();
                            _tooltip = item->GetTooltip();
                            assert(_tooltip);
                            _highlightItems[item->GetID()] = InteractiveItemHighlightColor;

                            _interactiveSelectionCursor = item->GetInteractCursor();
                            if (_interactiveSelectionCursor == nullptr)
                            {
                                _interactiveSelectionCursor = _defaultInteractCursor;
                            }

                            foundHighlight = true;
                        }
                    }

                    if (!foundHighlight)
                    {
                        // Scan for a tooltip
                        std::vector<Character*> charactersUnderMouse = primaryLayer->GetCharacters<Character>(
                            [&](const Character* character)
                            {
                                if (!character->Intersects(mousePosWorld))
                                {
                                    return false;
                                }

                                bool enemy = character->GetController() != this &&
                                    character->GetController() != nullptr &&
                                    (character->GetEntityMask() & CharacterMask_Aesthetic) == 0 &&
                                    character->IsAlive();
                                bool interactive = (character->GetController() == this || character->GetController() == nullptr) &&
                                    ((character->GetEntityMask() & CharacterMask_Usable) != 0) ||
                                    (character->GetEntityMask() & CharacterMask_Destructable) != 0;
                                if (!enemy && !interactive)
                                {
                                    return false;
                                }

                                if (!isCharacterHighlightable(character))
                                {
                                    return false;
                                }

                                return true;
                            });
                        if (!charactersUnderMouse.empty())
                        {
                            Character* character = charactersUnderMouse.back();
                            _tooltip = character->GetTooltip();
                            if (character->GetController() != this && character->GetController() != nullptr)
                            {
                                _highlightCharacters[character->GetID()] = AttackableCharacterHighlightColor;
                            }
                            else if ((character->GetEntityMask() & CharacterMask_Destructable) != 0)
                            {
                                _highlightCharacters[character->GetID()] = DestructableCharacterHighlightColor;
                            }
                            else if ((character->GetEntityMask() & CharacterMask_Usable) != 0)
                            {
                                _highlightCharacters[character->GetID()] = InteractiveCharacterHighlightColor;

                                _interactiveSelectionCursor = character->GetInteractCursor();
                                if (_interactiveSelectionCursor == nullptr)
                                {
                                    _interactiveSelectionCursor = _defaultInteractCursor;
                                }
                            }
                            else
                            {
                                assert(false);
                            }
                            foundHighlight = true;
                        }
                    }
                }
            }

            if (mouseOverUI)
            {
                if (_hudSelectionArea->IsMouseOver())
                {
                    // selection area draws its own tooltips in a custom location
                    //_tooltip = _hudSelectionArea->GetTooltip();
                }
                else if (_hudCharacterSelector->IsMouseOver())
                {
                    _tooltip = _hudCharacterSelector->GetTooltip();
                }
                else if (_hudPauseButton->IsMouseOver())
                {
                    _tooltip = _hudPauseButton->GetTooltip();
                }
                else if (_hudResourceDisplay->IsMouseOver())
                {
                    _tooltip = _hudResourceDisplay->GetTooltip();
                }
                else if (_hudMenuDisplay->IsMouseOver())
                {
                    _tooltip = _hudMenuDisplay->GetTooltip();
                }
            }

            SafeAddRef(_tooltip);
            SafeAddRef(_interactiveSelectionCursor);

            // Set highlight color for highlight characters
            for (auto highlightCharacter : _highlightCharacters)
            {
                auto character = primaryLayer->GetCharacter(highlightCharacter.first);
                if (character != nullptr)
                {
                    character->SetHighlightColor(highlightCharacter.second);
                }
            }

            for (auto highlightItem : _highlightItems)
            {
                auto item = primaryLayer->GetItem(highlightItem.first);
                if (item != nullptr)
                {
                    item->SetHighlightColor(highlightItem.second);
                }
            }

            if (!_paused && !mouseOverUI)
            {
                float deltaScroll = input.GetMouseScrollDelta().Y;
                if (deltaScroll != 0.0)
                {
                    _targetZoom = Clamp(_targetZoom * (1.0f + (deltaScroll * _zoomSpeed)), _minZoom, _maxZoom);
                    cameraController.ZoomOnTarget(_targetZoom, _mousePosWorld, 0.0f);
                }
                else
                {
                    _targetZoom = camera.GetZoom();
                }
            }

            if (!_paused && !_inCutscene)
            {

                for (uint32_t i = 0; i < _allCharacters.Count(); i++)
                {
                    BasicCharacter* character = AsA<BasicCharacter>(_allCharacters.GetCharacter(i));
                    if (character != nullptr)
                    {
                        if (_selectedCharacters.ContainsCharacter(character))
                        {
                            if (character->GetCurrentState() == CharacterState_Idle && character->WantsToLookAtMouse() && !character->IsInteracting())
                            {
                                character->SetRotation(Rotatorf(mousePosWorld - character->GetPosition()));
                            }

                            bool forward = Vector2f::Dot(character->GetRotation().ToVector(), mousePosWorld - character->GetPosition()) > 0.0f;
                            if (forward && !character->IsInteracting())
                            {
                                character->LookAt(mousePosWorld);
                            }
                            else
                            {
                                character->StopLooking();
                            }
                        }
                        else
                        {
                            character->StopLooking();
                        }
                    }
                }

                _timePlayingLevel += dt;

                for (uint32_t i = 0; i < _allCharacters.Count(); i++)
                {
                    Character* character = _allCharacters.GetCharacter(i);
                    if (character->GetAbilityCount() > 0)
                    {
                        _timeHavingAbilities += dt;
                        break;
                    }
                }

                // Tutorial checks
                static const float waitTimeForMoveTutorial = 1.5f;
                if (_timePlayingLevel > waitTimeForMoveTutorial && _allCharacters.Count() > 0)
                {
                    if (!_givenActionCommand && !_tutorialDisplay->HasShownTutorial(TutorialType::MovingCharacters))
                    {
                        Rectangleu clippedViewport = GetHUDClippedViewport(hudCamera.GetViewport());
                        Vector2f tutorialPos(clippedViewport.Position + (clippedViewport.Size / 4u));
                        _tutorialDisplay->ShowTutorial(TutorialType::MovingCharacters, tutorialPos);
                    }
                    else if (_givenActionCommand)
                    {
                        _tutorialDisplay->StopShowingTutorial(TutorialType::MovingCharacters);
                    }
                }

                static const float waitTimeForSelectionTutorial = 1.5f;
                if (_timePlayingLevel > waitTimeForSelectionTutorial && _allCharacters.Count() > 1)
                {
                    if (!_selectionChanged && !_tutorialDisplay->HasShownTutorial(TutorialType::CharacterSelection))
                    {
                        Rectangleu clippedViewport = GetHUDClippedViewport(hudCamera.GetViewport());
                        Vector2f tutorialPos(clippedViewport.Position + (clippedViewport.Size / 4u));
                        _tutorialDisplay->ShowTutorial(TutorialType::CharacterSelection, tutorialPos);
                    }
                    else if (_selectionChanged)
                    {
                        _tutorialDisplay->StopShowingTutorial(TutorialType::CharacterSelection);
                    }
                }

                static const float waitTimeForAbilityTutorial = 1.0f;
                if (_timeHavingAbilities > waitTimeForAbilityTutorial)
                {
                    if (!_abilityUsed && !_tutorialDisplay->HasShownTutorial(TutorialType::UseAbilities) &&
                        getPrimaryCharacter() && getPrimaryCharacter()->GetAbilityCount() > 0)
                    {
                        Rectanglef abilityButtonBounds = _hudSelectionArea->GetAbilityButtonBounds(0);
                        Vector2f tutorialPos = abilityButtonBounds.Middle() - Vector2f(abilityButtonBounds.W * 0.5f, 0.0f);
                        _tutorialDisplay->ShowTutorial(TutorialType::UseAbilities, tutorialPos);
                    }
                    else if (_abilityUsed)
                    {
                        _tutorialDisplay->StopShowingTutorial(TutorialType::UseAbilities);
                    }
                }
            }

            _hudActionDisplay->Update(totalTime, dt, camera, hudCamera);

            _affirmativeSoundResetTimer -= dt;
            _negatorySoundResetTimer -= dt;
        }

        void Player::OnDraw(Level::LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            if (layer->IsPrimary())
            {
                if (_mouseLightOn)
                {
                    Lights::PointLight mouseLight(_mousePosWorld, 2250.0f, Color::FromBytes(255, 204, 128, 255), 0.5f);
                    levelRenderer->AddLight(mouseLight);
                }

                const App::Cursor* cursor = getCursor();
                levelRenderer->SetCursor(cursor);
            }
        }

        void Player::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            if (_hudVisible)
            {
                // Draw selection rectangle
                if (_curContext == Context_Selecting)
                {
                    Level::LevelInstance* lvl = GetLevel();
                    const Camera& worldCam = lvl->GetCameraController().GetCamera();
                    const Camera& hudCam = lvl->GetHUDCamera();
                    DrawSelectionRectangle(spriteRenderer, worldCam, hudCam, getSelectionRectangle(), 2, Color::Green);
                }
            }

            _hudActionDisplay->Draw(spriteRenderer);

            // Draw hud elements
            _hudCharacterSelector->Draw(spriteRenderer);
            _hudSelectionArea->Draw(spriteRenderer);
            _minimap->Draw(spriteRenderer);
            _hudMenuDisplay->Draw(spriteRenderer);
            _hudPauseButton->Draw(spriteRenderer);
            _hudResourceDisplay->Draw(spriteRenderer);
            _tutorialDisplay->Draw(spriteRenderer);

            if (_tooltip)
            {
                const App::Cursor* cursor = getCursor();
                Vector2f minimumSize = _tooltip->GetMinimumSize(_uiScale);
                Vector2u cursorSize = cursor ? Vector2u(cursor->Width(), cursor->Height()) : Vector2u::Zero;
                const float tooltipCursorPadding = 3.0f;
                Rectanglef tooltipBounds(_mousePosScreen + Vector2f(0, cursorSize.Y + tooltipCursorPadding), minimumSize);
                _tooltip->Draw(spriteRenderer, Color::White, tooltipBounds, _uiScale);
            }
        }

        Rectanglef Player::getSelectionRectangle() const
        {
            Vector2f minPt = Vector2f::Min(_selectionOrigin, _selectionExtent);
            Vector2f maxPt = Vector2f::Max(_selectionOrigin, _selectionExtent);
            return Rectanglef(minPt, maxPt - minPt);
        }

        Character* Player::getPrimaryCharacter() const
        {
            return _selectedCharacters.GetCharacter(_selectedCharacterTypeIdx, _selectedCharacterIdx);
        }

        void Player::setContext(PlayerContext newContext)
        {
            if (_activeAbility)
            {
                _activeAbility->ClearState();
                _activeAbility = NULL;
                _setFirstAbilityPosition = false;
            }

            _curContext = newContext;
        }

        void Player::panCamera(const Vector2f& mousePos, const Rectanglef& viewport, const Input::FrameInput& input, float dt)
        {
            Level::LevelInstance* lvl = GetLevel();

            float zoomFactor = 1.0f / lvl->GetCameraController().GetCamera().GetZoom();

            Vector2f newCamPos = lvl->GetCameraController().GetCamera().GetPosition();
            bool doPan = false;

            if (input.IsBindPressed(_moveLeftBind) || Abs(mousePos.X - viewport.Left()) < _minScrollDist)
            {
                newCamPos.X -= _camMoveSpeed * zoomFactor * dt;
                doPan = true;
            }
            if (input.IsBindPressed(_moveRightBind) || Abs(mousePos.X - viewport.Right()) < _minScrollDist)
            {
                newCamPos.X += _camMoveSpeed * zoomFactor * dt;
                doPan = true;
            }
            if (input.IsBindPressed(_moveUpBind) || Abs(mousePos.Y - viewport.Top()) < _minScrollDist)
            {
                newCamPos.Y -= _camMoveSpeed * zoomFactor * dt;
                doPan = true;
            }
            if (input.IsBindPressed(_moveDownBind) || Abs(mousePos.Y - viewport.Bottom()) < _minScrollDist)
            {
                newCamPos.Y += _camMoveSpeed * zoomFactor * dt;
                doPan = true;
            }

            if (doPan)
            {
                lvl->GetCameraController().Pan(newCamPos);
            }
        }

        typedef std::function<Action(const Vector2f&)> MoveStyleActionGenFunc;
        static void MoveCharacters(std::vector<Character*>& characters, const Character* primary,
                                   const Vector2f& destination, bool queue, MoveStyleActionGenFunc func)
        {
            assert(!characters.empty());
            std::stable_sort(characters.begin(), characters.end(), [](const Character* a, const Character* b)
            {
                return a->GetPosition().X < b->GetPosition().X;
            });

            float spread = primary->GetBounds().W;

            Vector2f middle;
            Vector2f normal;
            if (!Level::GetMoveTarget(primary->GetLevelLayer(), primary, 0, destination, MaxMoveSearchDist, middle, normal))
            {
                middle = destination;
                normal = Vector2f::UnitY;
            }

            Vector2f tangent(normal.Y, normal.X);

            Vector2f a = middle - tangent * (spread * 0.5f);
            Vector2f b = middle + tangent * (spread * 0.5f);

            for (uint32_t i = 0; i < characters.size(); i++)
            {
                float perc = (characters.size() > 1) ? float(i) / (characters.size() - 1) : 0.5f;
                characters[i]->PushAction(func(Vector2f::Lerp(a, b, perc)), queue);
            }
        }

        void Player::SelectedCharactersMove(const Vector2f& destination, bool queue)
        {
            if (_selectedCharacters.Count() == 0)
            {
                return;
            }

            Level::LevelLayerInstance* layer = GetLevel()->GetPrimaryLayer();
            std::vector<Character*> allCharsAtDest = layer->FindIntersections<>(destination);

            std::vector<Character*> moveCharacters;
            moveCharacters.reserve(_selectedCharacters.Count());

            for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
            {
                Character* character = _selectedCharacters.GetCharacter(i);

                // Skip characters that can't move at all
                if (character->GetMoveType() == MoveType_None)
                {
                    continue;
                }

                // Check if we should be following instead of moving
                for (auto charAtDest = allCharsAtDest.rbegin(); charAtDest != allCharsAtDest.rend(); charAtDest++)
                {
                    if (character->IsCharacterFollowable(*charAtDest))
                    {
                        // Follow instead
                        for (uint32_t j = 0; j < _selectedCharacters.Count(); j++)
                        {
                            _selectedCharacters.GetCharacter(j)->PushAction(CreateFollowAction((*charAtDest)->GetID()), queue);
                        }
                        _hudActionDisplay->OnCharactersFollow((*charAtDest)->GetID());
                        PlaySelectedCharacterAffirmative();
                        _givenActionCommand = true;
                        return;
                    }
                }

                moveCharacters.push_back(character);
            }

            if (!moveCharacters.empty())
            {
                Vector2f middleMoveTarget;
                Vector2f middleMoveNormal;
                if (Level::GetMoveTarget(layer, getPrimaryCharacter(), 0, destination, MaxMoveSearchDist, middleMoveTarget, middleMoveNormal))
                {
                    _hudActionDisplay->OnCharactersMove(middleMoveTarget);
                    PlaySelectedCharacterAffirmative();
                }

                MoveCharacters(moveCharacters, getPrimaryCharacter(), destination, queue, [](const Vector2f& pos) { return CreateMoveAction(pos); });
                _givenActionCommand = true;
            }
        }

        void Player::SelectedCharactersAttack(const Vector2f& destination, bool forced, bool queue)
        {
            if (_selectedCharacters.Count() == 0)
            {
                return;
            }

            Level::LevelLayerInstance* layer = GetLevel()->GetPrimaryLayer();
            Character* primaryCharacter = getPrimaryCharacter();

            std::vector<Character*> moveCharacters;

            std::vector<Character*> charsAtDest = layer->GetCharacters<Character>([&](const Character* other)
            {
                return other->Intersects(destination) && isCharacterHighlightable(other);
            });
            std::vector<Character*> attakedCharacters;
            for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
            {
                bool gaveAttackCommand = false;

                Character* character = _selectedCharacters.GetCharacter(i);
                for (auto charAtDest = charsAtDest.rbegin(); charAtDest != charsAtDest.rend(); charAtDest++)
                {
                    if (character->IsCharacterAttackable(*charAtDest, forced))
                    {
                        attakedCharacters.push_back(*charAtDest);
                        character->PushAction(CreateAttackAction((*charAtDest)->GetID()), queue);
                        gaveAttackCommand = true;
                        break;
                    }
                }

                if (!gaveAttackCommand)
                {
                    moveCharacters.push_back(character);
                }
            }

            for (auto attakedChar : attakedCharacters)
            {
                _hudActionDisplay->OnCharactersAttack(attakedChar->GetID());
                PlaySelectedCharacterAffirmative();
                _givenActionCommand = true;
            }
            if (!moveCharacters.empty())
            {
                Vector2f moveTarget;
                Vector2f middleMoveNormal;
                if (Level::GetMoveTarget(layer, primaryCharacter, 0, destination, MaxMoveSearchDist, moveTarget, middleMoveNormal))
                {
                    _hudActionDisplay->OnCharactersAttackMove(moveTarget);
                    PlaySelectedCharacterAffirmative();
                }

                MoveCharacters(moveCharacters, primaryCharacter, moveTarget, queue, [](const Vector2f& pos) { return CreateAttackMoveAction(pos); });
                _givenActionCommand = true;
            }
        }

        void Player::SelectedCharactersInteract(const Vector2f& destination, bool queue)
        {
            if (_selectedCharacters.Count() == 0)
            {
                return;
            }

            Level::LevelLayerInstance* layer = GetLevel()->GetPrimaryLayer();

            std::vector<Character*> interactiveCharsAtDest = layer->GetCharacters<Character>([&](const Character* other)
            {
                return (other->GetEntityMask() & CharacterMask_Usable) != 0 && other->Intersects(destination) && isCharacterHighlightable(other);
            });
            std::vector<Item::Item*> interactiveItemsAtDest = layer->FindItems<Item::Item>([&](const Item::Item* other)
            {
                Physics::Collision* collision = other->GetCollision();
                if (collision == nullptr || !collision->Contains(destination))
                {
                    return false;
                }

                if (other->HasOwner())
                {
                    return false;
                }

                if (!isItemHighlightable(other))
                {
                    return false;
                }

                return true;
            });

            if (interactiveItemsAtDest.size() > 0 || interactiveCharsAtDest.size() > 0)
            {
                Character* closestToDest = nullptr;
                float closestDist = 0.0f;
                bool interactWithItem = false;
                for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
                {
                    Character* character = _selectedCharacters.GetCharacter(i);
                    float dist = 0.0f;
                    bool foundPath = false;
                    bool bestIsItem = false;
                    if (interactiveItemsAtDest.size() > 0 && character->IsItemInteractable(interactiveItemsAtDest.back()))
                    {
                        std::shared_ptr<Pathfinding::Path> path = character->ComputePath(
                            interactiveItemsAtDest.back()->GetPosition(), MaxMoveSearchDist);
                        if (path != nullptr)
                        {
                            foundPath = true;
                            dist = path->GetLength();
                            bestIsItem = true;
                        }
                    }
                    else if (interactiveCharsAtDest.size() > 0 && character->IsCharacterInteractable(interactiveCharsAtDest.back()))
                    {
                        std::shared_ptr<Pathfinding::Path> path = character->ComputePath(
                            interactiveCharsAtDest.back()->GetInteractionMoveTarget(character), MaxMoveSearchDist);
                        if (path != nullptr)
                        {
                            foundPath = true;
                            dist = path->GetLength();
                            bestIsItem = false;
                        }
                    }

                    if (foundPath && (closestToDest == nullptr || dist < closestDist))
                    {
                        closestToDest = character;
                        closestDist = dist;
                        interactWithItem = bestIsItem;
                    }
                }

                if (closestToDest != nullptr)
                {
                    if (interactWithItem)
                    {
                        assert(interactiveItemsAtDest.size() > 0);
                        CharacterID id = interactiveItemsAtDest.back()->GetID();
                        closestToDest->PushAction(CreateInteractItemAction(id), queue);
                        _hudActionDisplay->OnCharactersInteractItem(id);
                        interactiveItemsAtDest.pop_back();
                    }
                    else
                    {
                        assert(interactiveCharsAtDest.size() > 0);
                        CharacterID id = interactiveCharsAtDest.back()->GetID();
                        closestToDest->PushAction(CreateInteractCharacterAction(id), queue);
                        _hudActionDisplay->OnCharactersInteractCharacter(closestToDest->GetID(), id);
                        interactiveCharsAtDest.pop_back();
                    }

                    PlaySelectedCharacterAffirmative();
                    _givenActionCommand = true;
                }
            }
        }

        void Player::SelectedCharactersDoAction(const Vector2f& destination, bool queue)
        {
            Level::LevelLayerInstance* layer = GetLevel()->GetPrimaryLayer();

            // Try to interact or attack
            std::vector<Character*> allCharsAtDest = layer->GetCharacters<Character>([&](const Character* other)
            {
                return other->Intersects(destination) && isCharacterHighlightable(other);
            });
            std::vector<Item::Item*> interactiveItemsAtDest = layer->FindItems<Item::Item>([&](const Item::Item* other)
            {
                Physics::Collision* collision = other->GetCollision();
                return (other->IsInteractive() && collision != nullptr && collision->Contains(destination) && isItemHighlightable(other));
            });
            if (allCharsAtDest.size() > 0 || interactiveItemsAtDest.size() > 0)
            {
                bool attackable = false;
                bool interactable = false;

                if (interactiveItemsAtDest.size() > 0)
                {
                    interactable = true;
                }
                for (const auto& charAtDest : allCharsAtDest)
                {
                    for (uint32_t i = 0; i < _selectedCharacters.Count(); i++)
                    {
                        Character* selectedCharacter = _selectedCharacters.GetCharacter(i);
                        if (selectedCharacter == charAtDest)
                        {
                            continue;
                        }

                        if (selectedCharacter->IsCharacterInteractable(charAtDest))
                        {
                            interactable = true;
                            break;
                        }
                        if (selectedCharacter->IsCharacterAttackable(charAtDest, false))
                        {
                            attackable = true;
                        }
                    }
                }

                if (interactable)
                {
                    SelectedCharactersInteract(destination, queue);
                    return;
                }

                if (attackable)
                {
                    SelectedCharactersAttack(destination, false, queue);
                    return;
                }
            }

            // Fall back to moving
            SelectedCharactersMove(destination, queue);
        }

        void Player::PlaySelectedCharacterAffirmative()
        {
            if (_affirmativeSoundResetTimer >= 0.0f)
            {
                return;
            }

            BasicCharacter* primaryCharacter = AsA<BasicCharacter>(getPrimaryCharacter());
            if (primaryCharacter == nullptr)
            {
                return;
            }

            primaryCharacter->Speak(Speech_Affirmative, false);
            _affirmativeSoundResetTimer = Random::RandomBetween(AffirmitiveSoundResetRange.first, AffirmitiveSoundResetRange.second);
        }

        void Player::PlaySelectedCharacterNegatory()
        {
            if (_affirmativeSoundResetTimer >= 0.0f)
            {
                return;
            }

            BasicCharacter* primaryCharacter = AsA<BasicCharacter>(getPrimaryCharacter());
            if (primaryCharacter == nullptr)
            {
                return;
            }

            primaryCharacter->Speak(Speech_Negatory, false);
            _affirmativeSoundResetTimer = Random::RandomBetween(NegatorySoundResetRange.first, NegatorySoundResetRange.second);
        }

        bool Player::isCharacterHighlightable(const Character* target) const
        {
            if (target->GetController() == this)
            {
                return true;
            }

            for (uint32_t characterIdx = 0; characterIdx < _allCharacters.Count(); characterIdx++)
            {
                auto character = _allCharacters.GetCharacter(characterIdx);
                if (character->HasLineOfSight(target))
                {
                    return true;
                }
            }

            return false;
        }

        bool Player::isItemHighlightable(const Item::Item* target) const
        {
            if (target->HasOwner() && target->GetOwner()->GetController() == this)
            {
                return true;
            }

            for (uint32_t characterIdx = 0; characterIdx < _allCharacters.Count(); characterIdx++)
            {
                auto character = _allCharacters.GetCharacter(characterIdx);
                if (character->HasLineOfSight(target))
                {
                    return true;
                }
            }

            return false;
        }

        void Player::PositionHUDButtons(const Rectangleu& viewport, float scale)
        {
            Rectangleu clippedViewport = GetHUDClippedViewport(viewport);

            _hudSelectionArea->SetPosition(clippedViewport.BottomRight(), scale);
            _minimap->SetPosition(clippedViewport.BottomLeft(), scale);

            const Rectanglef& selectionArea = _hudSelectionArea->GetBounds();
            const Rectanglef& minimapArea = _minimap->GetBounds();
            _hudCharacterSelector->SetPosition(Vector2f(minimapArea.Right(), clippedViewport.Bottom()), Vector2f(clippedViewport.Right() - selectionArea.W, clippedViewport.Bottom()), scale);

            _hudMenuDisplay->SetScale(scale);
            _hudActionDisplay->SetScale(scale);

            _hudResourceDisplay->SetPosition(clippedViewport.TopRight(), scale);

            _hudPauseButton->SetPosition(clippedViewport.TopLeft(), scale);
            Vector2f pauseButtonSize = _hudPauseButton->GetBounds().Size;
            Vector2f pauseButtonPosition(clippedViewport.Left() + pauseButtonSize.X * 0.5f + pauseButtonSize.X * 0.1f, clippedViewport.Top());
            _hudPauseButton->SetPosition(pauseButtonPosition, scale);
        }

        void Player::DrawSelectionRectangle(Graphics::SpriteRenderer* spriteRenderer, const Camera& worldCam, const Camera& hudCam, const Rectanglef& rect,
                                            float thickness, const Color& color) const
        {
            if (rect.W > 1 && rect.H > 1)
            {
                Vector2f a = Camera::Transpose(worldCam, rect.TopLeft(), hudCam);
                Vector2f b = Camera::Transpose(worldCam, rect.TopRight(), hudCam);
                Vector2f c = Camera::Transpose(worldCam, rect.BottomRight(), hudCam);
                Vector2f d = Camera::Transpose(worldCam, rect.BottomLeft(), hudCam);

                spriteRenderer->DrawLine(a, b, thickness, color);
                spriteRenderer->DrawLine(b, c, thickness, color);
                spriteRenderer->DrawLine(c, d, thickness, color);
                spriteRenderer->DrawLine(d, a, thickness, color);
            }
        }

        const App::Cursor* Player::getCursor() const
        {
            if (_paused)
            {
                return _defaultCursor;
            }

            if (_curContext == Context_Ability)
            {
                assert(_activeAbility != nullptr);

                const App::Cursor* abilityCursor = _activeAbility->GetCursor();
                if (abilityCursor)
                {
                    return abilityCursor;
                }
            }
            else if (_curContext == Context_None)
            {
                if (_interactiveSelectionCursor != nullptr)
                {
                    return _interactiveSelectionCursor;
                }
            }

            auto iter = _cursors.find(_curContext);
            return (iter != _cursors.end()) ? iter->second : nullptr;
        }

        void Player::iterateSelectedCharacter()
        {
            uint32_t typeCount = _selectedCharacters.GetTypeCount();
            if (typeCount > 0)
            {
                uint32_t selectedTypeCount = _selectedCharacters.Count(_selectedCharacterTypeIdx);

                _selectedCharacterIdx++;
                if (_selectedCharacterIdx >= selectedTypeCount)
                {
                    _selectedCharacterTypeIdx = (_selectedCharacterTypeIdx + 1) % typeCount;
                    _selectedCharacterIdx = 0;
                }
            }
            else
            {
                _selectedCharacterTypeIdx = 0;
            }

            _primarySelectionChanged = true;
        }

        void Player::clearSelection()
        {
            _selectedCharacters.Clear();
            _selectedCharacterTypeIdx = 0;
            _selectedCharacterIdx = 0;
        }

        void Player::animateHUDIn(bool instant)
        {
            _hudCharacterSelector->AnimateIn(instant);
            _hudSelectionArea->AnimateIn(instant);
            _minimap->AnimateIn(instant);
            _hudPauseButton->AnimateIn(instant);
            _hudResourceDisplay->AnimateIn(instant);
            _tutorialDisplay->AnimateIn(instant);
            _hudVisible = true;
        }

        void Player::animateHUDOut(bool instant)
        {
            _hudCharacterSelector->AnimateOut(instant);
            _hudSelectionArea->AnimateOut(instant);
            _minimap->AnimateOut(instant);
            _hudPauseButton->AnimateOut(instant);
            _hudResourceDisplay->AnimateOut(instant);
            _tutorialDisplay->AnimateOut(instant);
            _hudVisible = false;
        }

        void Player::pause()
        {
            assert(!_paused);
            animateHUDOut(false);

            std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>> checkpoints;
            Level::CampaignLevel* level = AsA<Level::CampaignLevel>(GetLevel());
            if (level != nullptr)
            {
                checkpoints = level->GetCheckpoints();
            }

            _hudMenuDisplay->ShowPauseMenu(checkpoints);

            _paused = true;
        }

        void Player::unpause()
        {
            assert(_paused);
            if (!_inCutscene)
            {
                animateHUDIn(false);
            }
            _hudMenuDisplay->Hide();

            _paused = false;
        }

        void Player::OnLevelVictory()
        {
            animateHUDOut(false);

            _hudMenuDisplay->ShowVictoryMenu();

            _paused = true;
            _onVictoryScreen = true;
        }

        void Player::OnLevelFailed()
        {
            animateHUDOut(false);

            std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>> checkpoints;
            Level::CampaignLevel* level = AsA<Level::CampaignLevel>(GetLevel());
            if (level != nullptr)
            {
                checkpoints = level->GetCheckpoints();
            }

            _hudMenuDisplay->ShowDefeatMenu(checkpoints);

            _paused = true;
            _onDefeatScreen = true;
        }
    }

    template <>
    void EnumeratePreloads<Character::Player>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Controller>(preloads);

        EnumeratePreloads<HUD::ActionDisplay>(preloads);
        EnumeratePreloads<HUD::CharacterSelector>(preloads);
        EnumeratePreloads<HUD::Minimap>(preloads);
        EnumeratePreloads<HUD::SelectionArea>(preloads);
        EnumeratePreloads<HUD::MenuDisplay>(preloads);
        EnumeratePreloads<HUD::PauseButton>(preloads);
        EnumeratePreloads<HUD::ResourceDisplay>(preloads);
        EnumeratePreloads<HUD::Debugger>(preloads);

        preloads.insert("HUD/hud.hudmatset");
        preloads.insert("HUD/hud.cursorset");
    }
}
