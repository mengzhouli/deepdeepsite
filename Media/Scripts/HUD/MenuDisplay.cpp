#include "HUD/MenuDisplay.hpp"
#include "HUD/Tooltip.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        static const std::string MenuSkeletonPath = "Skeletons/UI/MenuDisplay/menu_display.skel";
        static const std::string MenuBaseMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_base.polymatset";
        static const std::string MenuVictoryMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_victory.polymatset";
        static const std::string MenuDefeatMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_defeat.polymatset";
        static const std::string MenuPausedMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_paused.polymatset";
        static const std::string MenuForwardHighlightMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_forward_highlight.polymatset";
        static const std::string MenuBackHighlightMatsetPath = "Skeletons/UI/MenuDisplay/menu_display_back_highlight.polymatset";

        static const std::string RootJoint = "origin";
        static const std::string CheckpointAreaPosJoint = "checkpoint_area_pos";
        static const std::string CheckpointAreaSizeJoint = "checkpoint_area_size";

        static const std::string ForwardJointName = "forward_button";
        static const std::string BackJointName = "back_button";

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        static const Audio::SoundPathVector ChainSoundPaths =
        {
            { "Audio/Characters/Chain/Chain_drop_1.wav", 1.0f },
            { "Audio/Characters/Chain/Chain_drop_2.wav", 1.0f },
        };

        static const std::vector<Audio::SoundPathVector> ThudSoundPaths =
        {
            {
                { "Audio/Footsteps/step_Land_OrcAlways_1.wav", 1.0f },
                { "Audio/Footsteps/step_Land_OrcAlways_2.wav", 1.0f },
            },
            {
                { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
            },
        };
        static const float SoundVolume = 0.5f;

        static const std::string HUDMaterialSetPath = "HUD/hud.hudmatset";
        static const std::string HUDScrollUpButtonMaterial = "scroll_up_button";
        static const std::string HUDScrollDownButtonMaterial = "scroll_down_button";
        static const std::string HUDScrollDragButtonMaterial = "scroll_drag_button";

        static const std::string RestartAtLoadoutMaterial = "checkpoint_return_to_loadout";
        static const std::string RestartAtCheckpointMaterial = "checkpoint_return_to_checkpoint";
        static const std::string RestartAtCheckpointClosedMaterial = "checkpoint_return_to_checkpoint_closed";
        static const std::string CheckpointDisabledMaterial = "checkpoint_disabled";

        static const std::string ReturnToLoadoutString = "pause_return_to_loadout_tooltip";
        static const std::string ReturnToCheckpointString = "pause_return_to_checkpoint_tooltip";

        static const std::string PausedForwardButtonString = "pause_pause_forward_button_tooltip";
        static const std::string PausedBackButtonString = "pause_pause_back_button_tooltip";
        static const std::string VictoryForwardButtonString = "pause_victory_forward_button_tooltip";
        static const std::string DefeatBackButtonString = "pause_defeat_back_button_tooltip";

        static const std::string HUDFontPath = "Fonts/hud_font.ttf";

        static const uint32_t CheckpointMaxCount = 5;

        MenuDisplay::MenuDisplay(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Settings::Profile* profile,
                                 Audio::SoundManager* soundManager, Input::InputBindCode clickBind)
            : _contentManager(contentManager)
            , _strings(strings)
            , _soundManager(soundManager)
            , _clickBind(clickBind)
            , _menuSkeleton(nullptr)
            , _menuBaseMaterialSet(nullptr)
            , _menuMaterials()
            , _buttons(nullptr)
            , _checkpointInfo()
            , _forwardButtonTooltips()
            , _backButtonTooltips()
            , _currentState(MenuType_None)
            , _showButtons(false)
            , _selectedTooltip(nullptr)
            , _forwardJustPressed(false)
            , _backJustPressed(false)
            , _restartJustPressed(false)
            , _checkpointJustPressed(false)
            , _pressedCheckpoint(0)
            , _scale()
        {
            _menuSkeleton = EmplaceResource(Content::CreateSkeletonInstance(contentManager, MenuSkeletonPath));
            _menuBaseMaterialSet = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuBaseMatsetPath));
            _menuMaterials[MenuType_Victory] = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuVictoryMatsetPath));
            _menuMaterials[MenuType_Defeat] = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuDefeatMatsetPath));
            _menuMaterials[MenuType_Paused] = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuPausedMatsetPath));
            _forwardHightlight = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuForwardHighlightMatsetPath));
            _backHightlight = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(MenuBackHighlightMatsetPath));

            _chainSounds.AddSounds(ChainSoundPaths);
            _chainSounds.LoadContent(contentManager);

            for (const auto& thudSoundPaths : ThudSoundPaths)
            {
                Audio::SoundSet thudSounds(thudSoundPaths);
                thudSounds.LoadContent(contentManager);

                _thudSounds.push_back(thudSounds);
            }

            _buttons = MakeResource<HUD::ButtonPanel>();
            _buttons->SetClickBind(_clickBind);

            ResourcePointer<const Graphics::HUDMaterialSet> hudMatSet = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(HUDMaterialSetPath));
            ResourcePointer<PanelDrawable> buttonHightlight = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial("button_highlight"));
            ResourcePointer<PanelDrawable> scrollUpButton = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial("scroll_up_button"));
            ResourcePointer<PanelDrawable> scrollDownButton = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial("scroll_down_button"));
            ResourcePointer<PanelDrawable> scrollDragButton = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial("scroll_drag_button"));
            ResourcePointer<BorderPanelDrawable> buttonBorder = MakeResource<StretchingBorderPanelDrawable>(hudMatSet, "button_frame", "button_frame_inner");

            _restartAtLoadoutIcon = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial(RestartAtLoadoutMaterial));
            _restartAtCheckpointIcon = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial(RestartAtCheckpointMaterial));
            _restartAtCheckpointClosedIcon = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial(RestartAtCheckpointClosedMaterial));
            _disabledCheckpointIcon = MakeResource<HUDMaterialPanelDrawable>(hudMatSet->GetMaterial(CheckpointDisabledMaterial));

            _restartToLoadoutTooltip = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(ReturnToLoadoutString)));
            for (uint32_t i = 0; i < CheckpointMaxCount; i++)
            {
                _restartAtCheckpointTooltips.push_back(EmplaceResource(CreateSimpleTextTooltip(contentManager, Format(strings->GetString(ReturnToCheckpointString).c_str(), i + 1))));
                _restartAtCheckpointClosedTooltips.push_back(nullptr);
            }
            _disabledCheckpointTooltip = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(ReturnToLoadoutString)));

            _buttons->SetBorderMaterial(buttonBorder);
            _buttons->SetHighlightMaterial(buttonHightlight);
            //_buttons->SetScrollMaterials(scrollUpButton, scrollDownButton, scrollDragButton);

            _checkpointInfo.resize(CheckpointMaxCount + 1);
            _checkpointInfo[0].isRestart = true;
            _checkpointInfo[0].tooltip = nullptr;
            _checkpointInfo[0].valid = true;
            _checkpointInfo[0].icon = nullptr;
            for (uint32_t i = 0; i < CheckpointMaxCount; i++)
            {
                _checkpointInfo[i + 1].isRestart = false;
                _checkpointInfo[i + 1].checkpointIdx = i;
                _checkpointInfo[i + 1].tooltip = nullptr;
                _checkpointInfo[i + 1].valid = false;
                _checkpointInfo[i + 1].icon = nullptr;
            }

            _buttons->SetButtonCount(CheckpointMaxCount + 1, 3);

            _forwardButtonTooltips[MenuType_Victory] = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(VictoryForwardButtonString)));
            _forwardButtonTooltips[MenuType_Paused] = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(PausedForwardButtonString)));

            _backButtonTooltips[MenuType_Defeat] = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(DefeatBackButtonString)));
            _backButtonTooltips[MenuType_Paused] = EmplaceResource(CreateSimpleTextTooltip(contentManager, strings->GetString(PausedBackButtonString)));
        }

        MenuDisplay::~MenuDisplay()
        {
            _chainSounds.UnloadContent();
            for (auto& thudSounds : _thudSounds)
            {
                thudSounds.UnloadContent();
            }
        }

        void MenuDisplay::SetScale(float scale)
        {
            _menuSkeleton->SetScale(scale);
            _scale = scale;
        }

        void MenuDisplay::ShowVictoryMenu()
        {
            setState(MenuType_Victory);
            clearCheckpoints();
            _showButtons = false;
        }

        void MenuDisplay::ShowDefeatMenu(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints)
        {
            setState(MenuType_Defeat);
            populateCheckpoints(checkpoints);
            _showButtons = true;
        }

        void MenuDisplay::ShowPauseMenu(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints)
        {
            setState(MenuType_Paused);
            populateCheckpoints(checkpoints);
            _showButtons = true;
        }

        void MenuDisplay::Hide()
        {
            setState(MenuType_None);
        }

        bool MenuDisplay::IsMouseOver() const
        {
            return _mouseOver;
        }

        bool MenuDisplay::IsBackButtonJustPressed() const
        {
            return _backJustPressed;
        }

        bool MenuDisplay::IsForwardButtonJustPressed() const
        {
            return _forwardJustPressed;
        }

        bool MenuDisplay::IsRestartAtLoadoutButtonPressed() const
        {
            return _restartJustPressed;
        }

        bool MenuDisplay::IsCheckpointButtonPressed(uint32_t& outCheckpointIdx) const
        {
            outCheckpointIdx = _pressedCheckpoint;
            return _checkpointJustPressed;
        }
        const Panel* MenuDisplay::GetTooltip() const
        {
            return _selectedTooltip;
        }

        static RotatedRectanglef getRotatedRectangle(const Animation::SkeletonInstance* skeleton, const std::string& rootJoint, const std::string& posJoint,
                                                     const std::string& sizeJoint)
        {
            Rotatorf rotation = -skeleton->GetJointTransformation(rootJoint).Rotation;
            const Vector2f& posA = skeleton->GetJointPosition(posJoint);
            const Vector2f& posB = skeleton->GetJointPosition(sizeJoint);
            Vector2f origin = (posA + posB) * 0.5f;

            Vector2f rotatedA = Vector2f::Rotate(origin, posA, -rotation.Angle);
            Vector2f rotatedB = Vector2f::Rotate(origin, posB, -rotation.Angle);
            Vector2f extents = Vector2f::Abs(rotatedB - rotatedA) * 0.5f;

            return RotatedRectanglef(origin, extents, rotation);
        }

        void MenuDisplay::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            _selectedTooltip.Reset();

            _menuSkeleton->ClearMaterials(_forwardHightlight);
            _menuSkeleton->ClearMaterials(_backHightlight);

            Vector2f mousePos = input.UnProjectMousePosition(camera);

            const Rectanglef& viewport = camera.GetViewport();
            _menuSkeleton->SetPosition(Vector2f(viewport.Middle().X, viewport.Y));

            _menuSkeleton->Update(totalTime, dt);

            _buttons->SetBounds(getRotatedRectangle(_menuSkeleton, RootJoint, CheckpointAreaPosJoint, CheckpointAreaSizeJoint), _scale);
            _buttons->Update(input, camera);

            bool justClicked = input.IsBindJustPressed(_clickBind);

            bool mouseOverForward = _menuSkeleton->JointContainsPoint(ForwardJointName, mousePos);
            if (mouseOverForward)
            {
                _selectedTooltip = getForwardTooltip(_currentState);
                if (_selectedTooltip)
                {
                    _menuSkeleton->MergeMaterialSet(_forwardHightlight);
                }
            }
            _forwardJustPressed = justClicked && mouseOverForward;

            bool mouseOverBack = _menuSkeleton->JointContainsPoint(BackJointName, mousePos);
            _backJustPressed = justClicked && mouseOverBack;
            if (mouseOverBack)
            {
                _selectedTooltip = getBackTooltip(_currentState);
                if (_selectedTooltip)
                {
                    _menuSkeleton->MergeMaterialSet(_backHightlight);
                }
            }

            uint32_t mouseOverButton = 0;
            bool mouseOverCheckpoint = _buttons->IsButtonMouseOver(mouseOverButton);
            if (_showButtons && mouseOverCheckpoint)
            {
                _restartJustPressed = justClicked && _checkpointInfo[mouseOverButton].isRestart;
                _checkpointJustPressed = justClicked && !_checkpointInfo[mouseOverButton].isRestart && _checkpointInfo[mouseOverButton].valid;
                _pressedCheckpoint = _checkpointInfo[mouseOverButton].checkpointIdx;

                if (_checkpointInfo[mouseOverButton].valid)
                {
                    _selectedTooltip = _checkpointInfo[mouseOverButton].tooltip;
                }
            }
            else
            {
                _restartJustPressed = false;
                _checkpointJustPressed = false;
                _pressedCheckpoint = 0;
            }

            if (_menuSkeleton->HasAnimationTagJustPassed("chain"))
            {
                _soundManager->PlaySingleGlobalSound(_chainSounds.GetNextSound(), Audio::SoundPriority::High, SoundVolume);
            }
            if (_menuSkeleton->HasAnimationTagJustPassed("thud"))
            {
                for (const auto& thudSound : _thudSounds)
                {
                    _soundManager->PlaySingleGlobalSound(thudSound.GetNextSound(), Audio::SoundPriority::High, SoundVolume);
                }
            }

            _mouseOver = _menuSkeleton->ContainsPoint(mousePos);
        }

        void MenuDisplay::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            if (_showButtons)
            {
                const RotatedRectanglef& backgroundArea = _buttons->GetBounds();
                Rectanglef rectBounds(backgroundArea.Middle() - backgroundArea.Extents, backgroundArea.Extents * 2.0f);
                const Rotatorf& rotation = backgroundArea.R;
                spriteRenderer->DrawSprite(rectBounds, Color::Black, rotation);

                _buttons->Draw(spriteRenderer, Color::White);
            }
            _menuSkeleton->Draw(spriteRenderer);
        }

        void MenuDisplay::setState(MenuType state)
        {
            _currentState = state;
            if (state == MenuType_None)
            {
                _menuSkeleton->PlayAnimation(MoveOutAnimation, false, 0.25f, 0.0f);
            }
            else
            {
                _menuSkeleton->ClearMaterials();
                _menuSkeleton->MergeMaterialSet(_menuBaseMaterialSet);
                _menuSkeleton->MergeMaterialSet(_menuMaterials[_currentState]);
                _menuSkeleton->PlayAnimation(MoveInAnimation, false, 0.25f, 0.0f);
            }
        }

        void MenuDisplay::populateCheckpoints(const std::vector<std::shared_ptr<Level::CampaignLevelCheckpoint>>& checkpoints)
        {
            _checkpointInfo[0].valid = true;
            _checkpointInfo[0].tooltip = _restartToLoadoutTooltip;
            _checkpointInfo[0].icon = _restartAtLoadoutIcon;
            for (uint32_t i = 0; i < _checkpointInfo.size() - 1; i++)
            {
                bool valid = i < checkpoints.size();
                bool captured = valid && checkpoints[i] != nullptr;

                _checkpointInfo[i + 1].valid = captured;

                if (captured)
                {
                    _checkpointInfo[i + 1].tooltip = EmplaceResource(CreateSimpleTextTooltip(_contentManager, _strings->GetString(checkpoints[i]->GetNameCode())));
                    _checkpointInfo[i + 1].icon = _restartAtCheckpointIcon;
                }
                else if (valid)
                {
                    _checkpointInfo[i + 1].tooltip = _restartAtCheckpointClosedTooltips[i];
                    _checkpointInfo[i + 1].icon = _restartAtCheckpointClosedIcon;
                }
                else
                {
                    _checkpointInfo[i + 1].tooltip = _disabledCheckpointTooltip;
                    _checkpointInfo[i + 1].icon = _disabledCheckpointIcon;
                }
            }

            for (uint32_t i = 0; i < _checkpointInfo.size(); i++)
            {
                _buttons->SetButtonEnabled(i, _checkpointInfo[i].valid);
                _buttons->SetButtonIcon(i, _checkpointInfo[i].icon);
            }
        }

        void MenuDisplay::clearCheckpoints()
        {
            for (uint32_t i = 0; i < _checkpointInfo.size(); i++)
            {
                _checkpointInfo[i].valid = false;
                _buttons->SetButtonEnabled(i, false);
                _checkpointInfo[i].tooltip.Reset();
                _checkpointInfo[i].icon.Reset();
            }
        }

        const Panel* MenuDisplay::getForwardTooltip(MenuType state) const
        {
            auto iter = _forwardButtonTooltips.find(state);
            if (iter != _forwardButtonTooltips.end())
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }

        const Panel* MenuDisplay::getBackTooltip(MenuType state) const
        {
            auto iter = _backButtonTooltips.find(state);
            if (iter != _backButtonTooltips.end())
            {
                return iter->second;
            }
            else
            {
                return nullptr;
            }
        }
    }

    template <>
    void EnumeratePreloads<HUD::MenuDisplay>(PreloadSet& preloads)
    {
        preloads.insert(HUD::MenuSkeletonPath);
        preloads.insert(HUD::MenuBaseMatsetPath);
        preloads.insert(HUD::MenuVictoryMatsetPath);
        preloads.insert(HUD::MenuDefeatMatsetPath);
        preloads.insert(HUD::MenuPausedMatsetPath);
        preloads.insert(HUD::MenuForwardHighlightMatsetPath);
        preloads.insert(HUD::MenuBackHighlightMatsetPath);
        EnumerateTooltipPreloads(preloads);

        for (const auto& thudSound : HUD::ThudSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, thudSound);
        }
        EnumerateSoundVectorPreloads(preloads, HUD::ChainSoundPaths);
    }
}
