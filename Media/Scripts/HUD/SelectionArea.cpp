#include "HUD/SelectionArea.hpp"

#include "Graphics/Text/PreparedText.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Ability/Ability.hpp"
#include "Characters/SkeletonCharacter.hpp"

#include "HUD/Tooltip.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace HUD
    {
        struct SelectionAreaParams
        {
            std::string SkeletonPath;
            std::string MatsetPath;
            bool ShowActionButtons;
        };
        static const std::map<SelectionAreaType, SelectionAreaParams> SelectionAreaTypes
        {
            {
                SelectionAreaType::Classic,
                {
                    "Skeletons/UI/AbilityPanel/ability_panel.skel",
                    "Skeletons/UI/AbilityPanel/ability_panel.polymatset",
                    true,
                }
            },
            {
                SelectionAreaType::Large,
                {
                    "Skeletons/UI/AbilityPanel/ability_panel_large.skel",
                    "Skeletons/UI/AbilityPanel/ability_panel_large.polymatset",
                    false,
                }
            },
            {
                SelectionAreaType::Small,
                {
                    "Skeletons/UI/AbilityPanel/ability_panel_small.skel",
                    "Skeletons/UI/AbilityPanel/ability_panel_small.polymatset",
                    false,
                }
            },
        };

        static const std::string NameFontPath = "Fonts/hud_font.ttf";
        static const std::string TooltipFontPath = "Fonts/hud_font.ttf";

        static const std::string PanelRoot = "origin";
        static const std::pair<std::string, std::string> PanelButtonArea { "abilities_pos", "abilities_size" };
        static const std::pair<std::string, std::string> PanelNameArea { "name_pos", "name_size" };
        static const std::pair<std::string, std::string> PanelPortraitArea{ "portrait_pos", "portrait_size" };
        static const std::pair<std::string, std::string> PanelBackgroundArea{ "background_pos", "background_size" };
        static const std::string PanelTooltipPos = "tooltip_pos";

        static const std::string MoveInAnimation = "move_in";
        static const std::string MoveOutAnimation = "move_out";

        static const std::string AbilityIconMatsetPath = "HUD/hud.hudmatset";

        static const std::vector<std::string> AbilityKeyBinds
        {
            "ability_0",
            "ability_1",
            "ability_2",
            "ability_3",
        };

        static RotatedRectanglef getRotatedRectangle(const Animation::SkeletonInstance* skeleton, const std::string& rootJoint,
                                                     const std::pair<std::string, std::string>& jointNames)
        {
            Rotatorf rotation = -skeleton->GetJointTransformation(rootJoint).Rotation;
            const Vector2f& posA = skeleton->GetJointPosition(jointNames.first);
            const Vector2f& posB = skeleton->GetJointPosition(jointNames.second);
            Vector2f origin = (posA + posB) * 0.5f;

            Vector2f rotatedA = Vector2f::Rotate(origin, posA, -rotation.Angle);
            Vector2f rotatedB = Vector2f::Rotate(origin, posB, -rotation.Angle);
            Vector2f extents = Vector2f::Abs(rotatedB - rotatedA) * 0.5f;

            return RotatedRectanglef(origin, extents, rotation);
        }

        static std::string getButtonKeybindText(const Localization::StringTable* strings, const Settings::Profile* profile, const std::string& bindName)
        {
            const Input::InputBinding& bind = profile->GetBind(bindName);
            if (bind.Type != Input::BindType_Unbound)
            {
                const std::string& inputNameCode = Input::InputBinding::GetInputName(bind);
                const std::string& localizedName = strings->GetString(inputNameCode);
                return Format("<color=white>%s</color>", localizedName.c_str());
            }
            else
            {
                return "";
            }
        }

        SelectionArea::SelectionArea(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Settings::Profile* profile,
            Graphics::Portrait* portrait, Input::InputBindCode clickBind, SelectionAreaType type)
            : _type(type)

            , _clickBind(clickBind)

            , _portrait(portrait)

            , _stringTable(strings)
            , _profile(profile)

            , _selectedCharacter(nullptr)
            , _selectedCharacterSize()
            , _selectedCharacterSkeleton(nullptr)

            , _mouseOver(false)
            , _buttonJustClicked(false)
            , _clickedButton(0)

            , _portraitClicked(false)

            , _selectedTooltip(nullptr)

            , _nameFont(nullptr)
            , _tooltipFont(nullptr)
            , _panel(nullptr)
            , _buttons(nullptr)
            , _tooltips()

            , _scale(1.0f)
        {
            assert(_portrait && _stringTable && contentManager);
            SafeAddRef(_stringTable);
            SafeAddRef(_portrait);

            if (SelectionAreaTypes.at(_type).ShowActionButtons)
            {
                _actionButtons =
                {
                    // Material name, name string, tooltip string
                    { "icon_attack", "action_attack_name", "action_attack_tooltip", "attack" },
                    { "icon_move", "action_move_name", "action_move_tooltip", "move" },
                    { "icon_stop", "action_stop_name", "action_stop_tooltip", "stop" },
                    { "icon_holdposition", "action_hold_name", "action_hold_tooltip", "hold_position" },
                };
            }
            _tooltips.resize(_actionButtons.size() + AbilityKeyBinds.size(), nullptr);

            _nameFont = contentManager->Load<Graphics::Font>(NameFontPath);
            _tooltipFont = contentManager->Load<Graphics::Font>(TooltipFontPath);

            _panel = Content::CreateSkeletonInstance(contentManager, SelectionAreaTypes.at(_type).SkeletonPath, SelectionAreaTypes.at(_type).MatsetPath);
            _panel->PlayAnimation(MoveOutAnimation, false, 0.0f, _panel->GetAnimationLength(MoveOutAnimation));

            BorderPanelDrawable* buttonBorder = CreateButtonBorder(contentManager);
            BorderPanelDrawable* buttonCornerBorder = CreateButtonCornerBorder(contentManager);
            PanelDrawable* buttonHighlight = CreateButtonHighlight(contentManager);

            const Graphics::HUDMaterialSet* buttonMaterials = contentManager->Load<Graphics::HUDMaterialSet>(AbilityIconMatsetPath);

            _buttons = new HUD::ButtonPanel();
            _buttons->SetButtonCount(_actionButtons.size() + AbilityKeyBinds.size(), AbilityKeyBinds.size());
            _buttons->SetBorderMaterial(buttonBorder);
            _buttons->SetCornerBorderMaterial(buttonCornerBorder);
            _buttons->SetHighlightMaterial(buttonHighlight);
            _buttons->SetClickBind(_clickBind);
            for (uint32_t i = 0; i < _actionButtons.size(); i++)
            {
                const std::string& materialName = _actionButtons[i].icon;
                const Graphics::HUDMaterial* material = buttonMaterials->GetMaterial(materialName);
                HUD::PanelDrawable* drawable = new HUD::HUDMaterialPanelDrawable(material);
                _buttons->SetButtonIcon(i, drawable);

                const std::string& actionCode = _actionButtons[i].nameStringCode;
                const std::string& tooltipCode = _actionButtons[i].tooltipStringCode;
                _tooltips[i] = CreateBasicActionTooltip(contentManager, _stringTable->GetString(actionCode), drawable, _stringTable->GetString(tooltipCode));

                SafeRelease(drawable);
            }

            SafeRelease(buttonBorder);
            SafeRelease(buttonCornerBorder);
            SafeRelease(buttonHighlight);
            SafeRelease(buttonMaterials);

            RotatedRectanglef portraitArea = getRotatedRectangle(_panel, PanelRoot, PanelPortraitArea);
            _portrait->SetSize(Vector2u(portraitArea.Width(), portraitArea.Height()));
        }

        SelectionArea::~SelectionArea()
        {
            SafeRelease(_selectedTooltip);
            SafeRelease(_portrait);

            SafeRelease(_stringTable);

            SafeRelease(_selectedCharacter);
            SafeRelease(_selectedCharacterSkeleton);

            SafeRelease(_nameFont);
            SafeRelease(_tooltipFont);
            SafeRelease(_panel);
            SafeRelease(_buttons);
            SafeReleaseAndClearContainer(_tooltips);
        }

        void SelectionArea::SetPosition(const Vector2f& pos, float scale)
        {
            _panel->SetPosition(pos);
            _panel->SetScale(scale);
            _scale = scale;
        }

        void SelectionArea::SetSelectedCharacter(Character::Character* character)
        {
            if (_selectedCharacter != character || character == nullptr)
            {
                SafeRelease(_selectedCharacter);
                SafeRelease(_selectedCharacterSkeleton);
                _selectedCharacter = character;
                SafeAddRef(_selectedCharacter);

                if (_portrait != nullptr)
                {
                    if (IsA<Character::SkeletonCharacter>(_selectedCharacter))
                    {
                        _selectedCharacterSkeleton = AsA<Character::SkeletonCharacter>(_selectedCharacter)->GetSkeleton();
                        SafeAddRef(_selectedCharacterSkeleton);
                        _selectedCharacterSize = _selectedCharacter->GetBounds().Size.Y;
                    }
                    _portrait->SetDrawable(_selectedCharacterSkeleton);
                }
            }
        }

        void SelectionArea::AnimateIn(bool instant)
        {
            _panel->PlayAnimation(MoveInAnimation, false, 0.0f, instant ? _panel->GetAnimationLength(MoveInAnimation) : 0.0f);
        }

        void SelectionArea::AnimateOut(bool instant)
        {
            _panel->PlayAnimation(MoveOutAnimation, false, 0.0f, instant ? _panel->GetAnimationLength(MoveOutAnimation) : 0.0f);
        }

        const Rectanglef& SelectionArea::GetBounds() const
        {
            return _panel->GetDrawBounds();
        }

        bool SelectionArea::IsMouseOver() const
        {
            return _mouseOver;
        }

        bool SelectionArea::IsPortraitClicked() const
        {
            return _portraitClicked;
        }

        bool SelectionArea::IsAttackButtonJustClicked() const
        {
            return _actionButtons.size() > 0 && _buttonJustClicked && _clickedButton == 0;
        }

        bool SelectionArea::IsMoveButtonJustClicked() const
        {
            return _actionButtons.size() > 1 && _buttonJustClicked && _clickedButton == 1;
        }

        bool SelectionArea::IsStopButtonJustClicked() const
        {
            return _actionButtons.size() > 2 && _buttonJustClicked && _clickedButton == 2;
        }

        bool SelectionArea::IsHoldPositionButtonJustClicked() const
        {
            return _actionButtons.size() > 3 && _buttonJustClicked && _clickedButton == 3;
        }

        bool SelectionArea::IsAbilityButtonJustClicked(uint32_t& outAbilityIdx) const
        {
            if (_buttonJustClicked && _clickedButton >= _actionButtons.size())
            {
                outAbilityIdx = _clickedButton - _actionButtons.size();
                return true;
            }
            else
            {
                return false;
            }
        }

        Rectanglef SelectionArea::GetAbilityButtonBounds(uint32_t abilityIdx) const
        {
            return _buttons->GetButtonBounds(_actionButtons.size() + abilityIdx).ToRectangle();
        }

        void SelectionArea::Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);

            for (uint32_t i = 0; i < _actionButtons.size(); i++)
            {
                _buttons->SetButtonVisible(i, _selectedCharacter != nullptr);
                _buttons->SetButtonCornerText(i, Alignment_BottomRight, _tooltipFont, getButtonKeybindText(_stringTable, _profile, _actionButtons[i].bind));
            }
            for (uint32_t i = 0; i < AbilityKeyBinds.size(); i++)
            {
                _buttons->SetButtonCornerText(_actionButtons.size() + i, Alignment_BottomRight, _tooltipFont, getButtonKeybindText(_stringTable, _profile, AbilityKeyBinds[i]));
            }
            if (_selectedCharacter != nullptr)
            {
                for (uint32_t i = 0; i < AbilityKeyBinds.size(); i++)
                {
                    SafeRelease(_tooltips[_actionButtons.size() + i]);

                    if (i < _selectedCharacter->GetAbilityCount())
                    {
                        Ability::Ability* ability = _selectedCharacter->GetAbility(i);

                        _buttons->SetButtonVisible(_actionButtons.size() + i, true);
                        _buttons->SetButtonIcon(_actionButtons.size() + i, ability->GetIcon());

                        _tooltips[_actionButtons.size() + i] = ability->GetTooltip();
                        SafeAddRef(_tooltips[_actionButtons.size() + i]);
                    }
                    else
                    {
                        _buttons->SetButtonVisible(_actionButtons.size() + i, false);
                        _buttons->SetButtonIcon(_actionButtons.size() + i, nullptr);
                    }
                }
            }
            else
            {
                for (uint32_t i = 0; i < AbilityKeyBinds.size(); i++)
                {
                    _buttons->SetButtonVisible(_actionButtons.size() + i, false);
                }
            }

            // Position elements
            _panel->Update(totalTime, dt);

            _buttons->SetBounds(getRotatedRectangle(_panel, PanelRoot, PanelButtonArea), _scale);
            _buttons->Update(input, camera);

            // Check input/tooltip
            SafeRelease(_selectedTooltip);
            uint32_t highlightedButton;
            if (_buttons->IsButtonHighlighted(highlightedButton))
            {
                _selectedTooltip = _tooltips[highlightedButton];
                SafeAddRef(_selectedTooltip);
            }

            _buttonJustClicked = _buttons->IsButtonJustClicked(_clickedButton);

            RotatedRectanglef portraitArea = getRotatedRectangle(_panel, PanelRoot, PanelPortraitArea);
            _portraitClicked = false;
            if (_selectedCharacterSkeleton != nullptr)
            {
                Vector2u portraitSize(portraitArea.Width(), portraitArea.Height());
                _portrait->SetSize(portraitSize);

                const Rectanglef& characterBounds = _selectedCharacter->GetBounds();
                _selectedCharacterSize = Max(_selectedCharacterSize, characterBounds.Size.Y);

                _portrait->SetScale(portraitSize.Y / _selectedCharacterSize * 0.8f);
                _portrait->SetPosition(_selectedCharacter->GetPosition() - Vector2f(0.0f, _selectedCharacterSize * 0.5f));
                _portrait->SetRotation(Rotatorf::Zero);

                _portraitClicked = input.IsBindPressed(_clickBind) && RotatedRectanglef::Contains(portraitArea, mousePos);
            }

            _mouseOver = _panel->ContainsPoint(mousePos);
        }

        void SelectionArea::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            {
                RotatedRectanglef backgroundArea = getRotatedRectangle(_panel, PanelRoot, PanelBackgroundArea);
                Rectanglef backgroundRect(backgroundArea.Middle() - backgroundArea.Extents, backgroundArea.Extents * 2.0f);
                spriteRenderer->DrawSprite(backgroundRect, Color::Black, backgroundArea.R);
            }

            _buttons->Draw(spriteRenderer, Color::White);

            if (_portrait != nullptr)
            {
                RotatedRectanglef portraitArea = getRotatedRectangle(_panel, PanelRoot, PanelPortraitArea);
                _portrait->Draw(spriteRenderer, portraitArea);
            }

            if (_selectedCharacter != nullptr)
            {
                RotatedRectanglef nameArea = getRotatedRectangle(_panel, PanelRoot, PanelNameArea);

                const std::string& nameText = _selectedCharacter->GetName();
                Graphics::PreparedText characterNameText = Graphics::FitTextToSize(nameText, _nameFont, Color::White, false, false, 1.0f, (nameArea.Extents * 2.0f) - Vector2f(16.0f, 0.0f));
                Vector2u textSize = characterNameText.GetSize();
                Vector2f textPosition = nameArea.Middle() - Vector2f(textSize.X * 0.5f, textSize.Y * 0.5f);

                spriteRenderer->DrawString(characterNameText, textPosition, Color::White, nameArea.R);
            }

            _panel->Draw(spriteRenderer);

            if (_selectedTooltip != nullptr)
            {
                Vector2f tooltipSize = _selectedTooltip->GetMinimumSize(_scale);
                Rectanglef tooltipBounds(_panel->GetJointPosition(PanelTooltipPos) - tooltipSize, tooltipSize);
                _selectedTooltip->Draw(spriteRenderer, Color::White, RotatedRectanglef(tooltipBounds, 0.0f), _scale);
            }
        }
    }

    template <>
    void EnumeratePreloads<HUD::SelectionArea>(PreloadSet& preloads)
    {
        EnumerateTooltipPreloads(preloads);

        preloads.insert(HUD::NameFontPath);

        for (auto selectionAreaType : HUD::SelectionAreaTypes)
        {
            preloads.insert(selectionAreaType.second.SkeletonPath);
            preloads.insert(selectionAreaType.second.MatsetPath);
        }
    }
}
