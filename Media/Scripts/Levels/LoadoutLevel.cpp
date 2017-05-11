#include "Levels/LoadoutLevel.hpp"

#include "Levels/CampaignLevel.hpp"

#include "Settings/TheDeepDeepProfile.hpp"

#include "HUD/Panel.hpp"

#include "Item/Weapon.hpp"
#include "Item/Armor.hpp"
#include "Item/Trinket.hpp"

#include "HUD/ButtonPanel.hpp"
#include "HUD/Tooltip.hpp"
#include "HUD/TutorialDisplay.hpp"

#include "Character/Controller.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"

#include "Characters/Torch.hpp"

#include "Items/GameItems.hpp"
#include "Items/Trinkets/Headlamp.hpp"

#include "Characters/GameCharacters.hpp"
#include "DwarfNameGenerator.hpp"

#include "Utility.hpp"
#include "ContentUtility.hpp"

#include <set>

namespace Dwarf
{
    struct DwarfTypeState
    {
        std::vector<Item::ItemInfo<Item::Weapon>> Weapons;
        std::vector<Item::ItemInfo<Item::Armor>> Armors;
        std::vector<Item::ItemInfo<Item::Trinket>> Trinkets;
    };

    struct LoadoutDwarfInfo
    {
        Character::Dwarf* Dwarf;
        uint32_t ProfileIndex;
        float Width;
        bool IsEquiped;
        std::set<uint32_t> WeaponIndex;
        std::set<uint32_t> ArmorIndex;
        std::set<uint32_t> TrinketIndex;

        LoadoutDwarfInfo()
            : LoadoutDwarfInfo(nullptr, 0)
        {
        };

        LoadoutDwarfInfo(Character::Dwarf* dwarf, uint32_t profileIndex)
            : Dwarf(dwarf)
            , ProfileIndex(profileIndex)
            , Width(dwarf->GetBounds().W * 0.65f)
            , IsEquiped(false)
            , WeaponIndex()
            , ArmorIndex()
            , TrinketIndex()
        {
        };
    };

    static const float ShowCreateDwarfTutorialStartTime = 3.0f;
    static const double GoButtonHintLightStrobeFrequency = 2.0f;
    static const double PortraitHintLightStrobeFrequency = 1.0f;

    namespace HUD
    {
        static const std::string LoadoutPanelSkeletonPath = "Skeletons/UI/Loadout/loadout_menu.skel";
        static const std::string LoadoutPanelMaterialPath = "Skeletons/UI/Loadout/loadout_menu.polymatset";
        static const std::string LoadoutPanelAcceptHighlightMaterialPath = "Skeletons/UI/Loadout/loadout_menu_accept_highlight.polymatset";
        static const std::string LoadoutPanelCancelHighlightMaterialPath = "Skeletons/UI/Loadout/loadout_menu_cancel_highlight.polymatset";

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
        static const float ChainSoundVolume = 0.5f;
        static const std::pair<float, float> ChainSoundRadius = { 1000.0f, 3000.0f };

        static const std::string LoadoutPanelWeaponPosJoint = "weapon_area_pos";
        static const std::string LoadoutPanelWeaponSizeJoint = "weapon_area_size";
        static const std::string LoadoutPanelTrinketPosJoint = "trinket_area_pos";
        static const std::string LoadoutPanelTrinketSizeJoint = "trinket_area_size";
        static const std::string LoadoutPanelArmorPosJoint = "armor_area_pos";
        static const std::string LoadoutPanelArmorSizeJoint = "armor_area_size";
        static const std::string LoadoutPanelBackgroundPosJoint = "background_pos";
        static const std::string LoadoutPanelBackgroundSizeJoint = "background_size";
        static const std::string LoadoutPanelNamePosJoint = "name_area_pos";
        static const std::string LoadoutPanelNameSizeJoint = "name_area_size";
        static const std::string LoadoutPanelAcceptJoint = "accept_button";
        static const std::string LoadoutPanelCancelJoint = "cancel_button";
        static const std::string LoadoutPanelRootJoint = "origin";
        static const std::string LoadoutPanelShowAnimation = "move_in";
        static const std::string LoadoutPanelHideAnimation = "move_out";

        static const std::string HUDMaterialSetPath = "HUD/hud.hudmatset";
        static const std::string HUDScrollUpButtonMaterial = "scroll_up_button";
        static const std::string HUDScrollDownButtonMaterial = "scroll_down_button";
        static const std::string HUDScrollDragButtonMaterial = "scroll_drag_button";

        static const std::string HUDFontPath = "Fonts/hud_font.ttf";

        class LoadoutPanel : public Resource, public Graphics::IDrawable
        {
        public:
            LoadoutPanel(Input::InputBindCode selectBind, const Localization::StringTable* strings, Audio::SoundManager* soundManager)
                : _soundManager(soundManager)
                , _position()
                , _uiScale()
                , _mouseOver(false)
                , _selectBind(selectBind)
                , _weaponPanel(nullptr)
                , _trinketPanel(nullptr)
                , _armorPanel(nullptr)
                , _menuSkeleton(nullptr)
                , _acceptHighlight(nullptr)
                , _cancelHighlight(nullptr)
                , _selectedDwarf(nullptr)
                , _selectedDwarfTypeState(nullptr)
                , _cursorSize()
                , _tooltip(nullptr)
                , _hudFont(nullptr)
            {
                _acceptText = strings->GetString("loadout_accept_tooltip");
                _cancelText = strings->GetString("loadout_cancel_tooltip");

                _weaponPanel = MakeResource<HUD::ButtonPanel>();
                _weaponPanel->SetClickBind(_selectBind);

                _trinketPanel = MakeResource<HUD::ButtonPanel>();
                _trinketPanel->SetClickBind(_selectBind);

                _armorPanel = MakeResource<HUD::ButtonPanel>();
                _armorPanel->SetClickBind(_selectBind);

                _chainSounds.AddSounds(ChainSoundPaths);
                for (const auto& thudSoundPaths : ThudSoundPaths)
                {
                    _thudSounds.push_back(Audio::SoundSet(thudSoundPaths));
                }
            }

            void Show()
            {
                if (_menuSkeleton && _menuSkeleton->GetCurrentAnimation() != LoadoutPanelShowAnimation)
                {
                    _menuSkeleton->PlayAnimation(LoadoutPanelShowAnimation, false, 0.15f);
                }
            }

            void Hide()
            {
                if (_menuSkeleton && _menuSkeleton->GetCurrentAnimation() != LoadoutPanelHideAnimation)
                {
                    _menuSkeleton->PlayAnimation(LoadoutPanelHideAnimation, false, 0.15f);
                }
            }

            void SetPosition(const Vector2f& pos, float skeletonScale, float uiScale)
            {
                _position = pos;
                _uiScale = uiScale;
                _skeletonScale = skeletonScale;
                if (_menuSkeleton)
                {
                    _menuSkeleton->SetPosition(_position);
                    _menuSkeleton->SetScale(_skeletonScale * _uiScale);
                }
            }

            void SetCursorSize(const Vector2f& size)
            {
                _cursorSize = size;
            }

            void SetSelectedDwarf(LoadoutDwarfInfo* dwarf, DwarfTypeState* typeState)
            {
                assert(dwarf && typeState);
                _selectedDwarf = dwarf;
                _selectedDwarfTypeState = typeState;

                Level::LevelLayerInstance* levelLayer = dwarf->Dwarf->GetLevelLayer();

                initializeItemPanel(levelLayer, _weaponPanel, _hudFont, typeState->Weapons, _sampleWeapons, _selectedDwarf->WeaponIndex);
                initializeItemPanel(levelLayer, _trinketPanel, _hudFont, typeState->Trinkets, _sampleTrinkets, _selectedDwarf->TrinketIndex);
                initializeItemPanel(levelLayer, _armorPanel, _hudFont, typeState->Armors, _sampleArmor, _selectedDwarf->ArmorIndex);
            }

            void UnsetSelectedDwarf()
            {
                _selectedDwarf = nullptr;
                _weaponPanel->SetButtonCount(0, 1);
                _trinketPanel->SetButtonCount(0, 1);
                _armorPanel->SetButtonCount(0, 1);
            }

            bool IsMouseOver() const
            {
                return _mouseOver;
            }

            bool IsJustClickingAccept() const
            {
                return _clickingAccept;
            }

            bool IsJustClickingCancel() const
            {
                return _clickingCancel;
            }

            const HUD::Panel* GetTooltip() const
            {
                return _tooltip;
            }

            void LoadContent(Content::ContentManager* contentManager)
            {
                _menuSkeleton = EmplaceResource(Content::CreateSkeletonInstance(contentManager, LoadoutPanelSkeletonPath, LoadoutPanelMaterialPath));
                _menuSkeleton->SetPosition(_position);
                _menuSkeleton->SetScale(_skeletonScale * _uiScale);
                _menuSkeleton->PlayAnimation(LoadoutPanelHideAnimation, false, 0.0f, _menuSkeleton->GetAnimationLength(LoadoutPanelHideAnimation));

                _chainSounds.LoadContent(contentManager);
                for (auto& thudSounds : _thudSounds)
                {
                    thudSounds.LoadContent(contentManager);
                }

                _acceptHighlight = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(LoadoutPanelAcceptHighlightMaterialPath));
                _cancelHighlight = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(LoadoutPanelCancelHighlightMaterialPath));

                _acceptTooltip = EmplaceResource(CreateSimpleTextTooltip(contentManager, _acceptText));
                _cancelTooltip = EmplaceResource(CreateSimpleTextTooltip(contentManager, _cancelText));

                ResourcePointer<PanelDrawable> scrollUpButton = EmplaceResource(CreateScrollUpButton(contentManager));
                ResourcePointer<PanelDrawable> scrollDownButton = EmplaceResource(CreateScrollDownButton(contentManager));
                ResourcePointer<PanelDrawable> scrollDragButton = EmplaceResource(CreateScrollDragButton(contentManager));
                ResourcePointer<BorderPanelDrawable> buttonBorder = EmplaceResource(CreateButtonBorder(contentManager));
                ResourcePointer<BorderPanelDrawable> buttonCornerBorder = EmplaceResource(CreateButtonCornerBorder(contentManager));
                ResourcePointer<PanelDrawable> buttonHighlight = EmplaceResource(CreateButtonHighlight(contentManager));

                _weaponPanel->SetBorderMaterial(buttonBorder);
                _weaponPanel->SetHighlightMaterial(buttonHighlight);
                _weaponPanel->SetCornerBorderMaterial(buttonCornerBorder);
                _weaponPanel->SetScrollMaterials(scrollUpButton, scrollDownButton, scrollDragButton);

                _trinketPanel->SetBorderMaterial(buttonBorder);
                _trinketPanel->SetHighlightMaterial(buttonHighlight);
                _trinketPanel->SetCornerBorderMaterial(buttonCornerBorder);
                _trinketPanel->SetScrollMaterials(scrollUpButton, scrollDownButton, scrollDragButton);

                _armorPanel->SetBorderMaterial(buttonBorder);
                _armorPanel->SetHighlightMaterial(buttonHighlight);
                _armorPanel->SetCornerBorderMaterial(buttonCornerBorder);
                _armorPanel->SetScrollMaterials(scrollUpButton, scrollDownButton, scrollDragButton);

                _acceptLight.Material = _menuSkeleton->GetMaterial(LoadoutPanelAcceptJoint);
                _cancelLight.Material = _menuSkeleton->GetMaterial(LoadoutPanelCancelJoint);

                _hudFont = EmplaceResource(contentManager->Load<Graphics::Font>(HUDFontPath));
            }

            void UnloadContent()
            {
                _menuSkeleton.Reset();
                _acceptHighlight.Reset();
                _cancelHighlight.Reset();

                _chainSounds.UnloadContent();
                for (auto& thudSounds : _thudSounds)
                {
                    thudSounds.UnloadContent();
                }

                _acceptTooltip.Reset();
                _cancelTooltip.Reset();

                _weaponPanel->SetButtonCount(0, 1);
                _trinketPanel->SetButtonCount(0, 1);
                _armorPanel->SetButtonCount(0, 1);

                _hudFont.Reset();
            }

            void Update(const Input::FrameInput& input, const Camera& camera, double totalTime, float dt)
            {
                _tooltip = nullptr;

                const Vector2f& mousePos = input.UnProjectMousePosition(camera);
                _mouseScreenSpace = mousePos;

                _menuSkeleton->Update(totalTime, dt);
                _menuSkeleton->ClearMaterials(_acceptHighlight);
                _menuSkeleton->ClearMaterials(_cancelHighlight);

                if (_menuSkeleton->HasAnimationTagJustPassed("chain"))
                {
                    _soundManager->PlaySinglePositionalSound(_chainSounds.GetNextSound(), Audio::SoundPriority::High, _menuSkeleton->GetJointPosition("chain_sound_source"), ChainSoundRadius.first, ChainSoundRadius.second, ChainSoundVolume);
                }
                if (_menuSkeleton->HasAnimationTagJustPassed("thud"))
                {
                    for (const auto& thudSound : _thudSounds)
                    {
                        _soundManager->PlaySinglePositionalSound(thudSound.GetNextSound(), Audio::SoundPriority::High, _menuSkeleton->GetJointPosition("thud_sound_source"), ChainSoundRadius.first, ChainSoundRadius.second, ChainSoundVolume);
                    }
                }

                updateItemPanel(_menuSkeleton, _weaponPanel, LoadoutPanelRootJoint, LoadoutPanelWeaponPosJoint, LoadoutPanelWeaponSizeJoint, input, camera, _uiScale);
                updateItemPanel(_menuSkeleton, _trinketPanel, LoadoutPanelRootJoint, LoadoutPanelTrinketPosJoint, LoadoutPanelTrinketSizeJoint, input, camera, _uiScale);
                updateItemPanel(_menuSkeleton, _armorPanel, LoadoutPanelRootJoint, LoadoutPanelArmorPosJoint, LoadoutPanelArmorSizeJoint, input, camera, _uiScale);

                const Animation::Transformation& acceptLightTransform = _menuSkeleton->GetJointTransformation(LoadoutPanelAcceptJoint);
                _acceptLight.Position = acceptLightTransform.Position;
                _acceptLight.Rotation = acceptLightTransform.Rotation;
                _acceptLight.Scale = acceptLightTransform.Scale.X;

                _mouseOver = _menuSkeleton->ContainsPoint(mousePos);
                if (_menuSkeleton->JointContainsPoint(LoadoutPanelAcceptJoint, mousePos))
                {
                    _clickingAccept = input.IsBindJustPressed(_selectBind);
                    _menuSkeleton->MergeMaterialSet(_acceptHighlight);
                    _tooltip = _acceptTooltip;
                    _acceptLight.LightColor = Color::FromBytes(64, 128, 64, 255);
                    _acceptLight.EmissiveColor = Color::FromBytes(128, 255, 128, 255);
                }
                else
                {
                    _clickingAccept = false;
                    _acceptLight.LightColor = Color::FromBytes(0, 0, 0, 0);
                    _acceptLight.EmissiveColor = Color::FromBytes(0, 0, 0, 0);
                }


                const Animation::Transformation& cancelLightTransform = _menuSkeleton->GetJointTransformation(LoadoutPanelCancelJoint);
                _cancelLight.Position = cancelLightTransform.Position;
                _cancelLight.Rotation = cancelLightTransform.Rotation;
                _cancelLight.Scale = cancelLightTransform.Scale.X;

                if (_menuSkeleton->JointContainsPoint(LoadoutPanelCancelJoint, mousePos))
                {
                    _clickingCancel = input.IsBindJustPressed(_selectBind);
                    _menuSkeleton->MergeMaterialSet(_cancelHighlight);
                    _tooltip = _cancelTooltip;
                    _cancelLight.LightColor = Color::FromBytes(128, 64, 64, 255);
                    _cancelLight.EmissiveColor = Color::FromBytes(255, 128, 128, 255);
                }
                else
                {
                    _clickingCancel = false;
                    _cancelLight.LightColor = Color::FromBytes(0, 0, 0, 0);
                    _cancelLight.EmissiveColor = Color::FromBytes(0, 0, 0, 0);
                }

                if (_selectedDwarf)
                {
                    checkPanelForClicks(_weaponPanel, _hudFont, _selectedDwarfTypeState->Weapons, _sampleWeapons, _selectedDwarf, _selectedDwarf->WeaponIndex, Item::ItemSlot::Weapon, _tooltip);
                    checkPanelForClicks(_trinketPanel, _hudFont, _selectedDwarfTypeState->Trinkets, _sampleTrinkets, _selectedDwarf, _selectedDwarf->TrinketIndex, Item::ItemSlot::Trinket, _tooltip);
                    checkPanelForClicks(_armorPanel, _hudFont, _selectedDwarfTypeState->Armors, _sampleArmor, _selectedDwarf, _selectedDwarf->ArmorIndex, Item::ItemSlot::Armor, _tooltip);
                }
            }

            const Rectanglef& GetDrawBounds() const override
            {
                return _menuSkeleton->GetDrawBounds();
            }

            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override
            {
                RotatedRectanglef backgroundRotatedRect = getRotatedRectangle(_menuSkeleton, LoadoutPanelRootJoint, LoadoutPanelBackgroundPosJoint,
                                                                              LoadoutPanelBackgroundSizeJoint);
                Rectanglef backgroundRect(backgroundRotatedRect.Middle() - backgroundRotatedRect.Extents, backgroundRotatedRect.Extents * 2.0f);
                spriteRenderer->DrawSprite(backgroundRect, Color::Black, backgroundRotatedRect.R, Vector2f(0.5f));

                _weaponPanel->Draw(spriteRenderer, Color::White);
                _trinketPanel->Draw(spriteRenderer, Color::White);
                _armorPanel->Draw(spriteRenderer, Color::White);
                _menuSkeleton->Draw(spriteRenderer);

                if (_selectedDwarf)
                {
                    RotatedRectanglef nameAreaRotatedRect = getRotatedRectangle(_menuSkeleton, LoadoutPanelRootJoint, LoadoutPanelNamePosJoint,
                                                                                LoadoutPanelNameSizeJoint);
                    Vector2f nameAreaSize = nameAreaRotatedRect.Extents * 2.0f;

                    // Draw the character's name
                    const std::string& name = _selectedDwarf->Dwarf->GetName();

                    uint32_t startTextSize = 12;
                    Graphics::PreparedText basicPreparedName = Graphics::PreparedText(name, _hudFont, startTextSize, Color::White, false, false);
                    Vector2u basicSize = basicPreparedName.GetSize();

                    float scale = (basicSize.X > 0 && basicSize.Y > 0) ? Min((nameAreaSize.X - 16) / basicSize.X, nameAreaSize.Y / basicSize.Y)
                                                                       : 1.0f;

                    Graphics::PreparedText characterNameText(name, _hudFont, uint32_t(startTextSize * scale), Color::White, false, false);
                    Vector2u textSize = characterNameText.GetSize();
                    Vector2f textPosition = nameAreaRotatedRect.Middle() - Vector2f(textSize.X * 0.5f, textSize.Y * 0.5f);

                    spriteRenderer->DrawString(characterNameText, textPosition, Color::White, nameAreaRotatedRect.R, Vector2f(0.5f));
                }
            }

            void Draw(Graphics::LevelRenderer* levelRenderer) const
            {
                levelRenderer->AddDrawable(this, false);
                levelRenderer->AddLight(_acceptLight);
                levelRenderer->AddLight(_cancelLight);
            }

        protected:
            virtual ~LoadoutPanel()
            {
            }

        private:
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

            static void clearSamples(std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>>& samples)
            {
                samples.clear();
            }

            template <typename T>
            static void initializeItemPanel(Level::LevelLayerInstance* levelLayer, ResourcePointer<ButtonPanel> panel, ResourcePointer<const Graphics::Font> font, const std::vector<Item::ItemInfo<T>>& itemInfo,
                std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>>& samples, std::set<uint32_t>& equippedItems)
            {
                samples.clear();
                panel->SetButtonCount(itemInfo.size(), 3);

                for (uint32_t i = 0; i < itemInfo.size(); i++)
                {
                    T* item = levelLayer->CreateItem(Vector2f::Zero, itemInfo[i].Constructor);

                    samples.push_back(std::make_pair(std::type_index(typeid(*item)), item->GetTooltip()));
                    panel->SetButtonIcon(i, item->GetIcon());
                    item->Terminate();

                    updateCornerText(panel, font, i, itemInfo[i].Count, equippedItems);
                }
            }

            static void updateItemPanel(ResourcePointer<const Animation::SkeletonInstance> skeleton, ButtonPanel* panel, const std::string& rootJoint,
                                        const std::string& posJoint, const std::string& sizeJoint, const Input::FrameInput& input, const Camera& camera,
                                        float scale)
            {
                panel->SetBounds(getRotatedRectangle(skeleton, rootJoint, posJoint, sizeJoint), scale);
                panel->Update(input, camera);
            }

            template <typename T>
            static void checkPanelForClicks(ButtonPanel* panel, const Graphics::Font* font, std::vector<Item::ItemInfo<T>>& itemInfo, const std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>>& samples,
                                            LoadoutDwarfInfo* selectedDwarf, std::set<uint32_t>& equippedItems, Item::ItemSlot slot, ResourcePointer<const HUD::Panel>& curTooltip)
            {
                uint32_t clickedButton;
                if (panel->IsButtonJustClicked(clickedButton))
                {
                    auto playEquipSound = [&](const T* item)
                    {
                        assert(item);
                        const Audio::Sound* sound = item->GetSelectionSound();
                        Audio::SoundManager* soundManager = item->GetLevel()->GetSoundManager();
                        soundManager->PlaySingleGlobalSound(sound, Audio::SoundPriority::High, 1.0f);
                    };

                    bool equipped = equippedItems.find(clickedButton) != equippedItems.end();
                    if (!equipped && itemInfo[clickedButton].Count > 0)
                    {
                        std::vector<T*> curItems = selectedDwarf->Dwarf->GetItems<T>(slot);
                        bool hasRoom = (curItems.size() < selectedDwarf->Dwarf->GetItemSlotCount(slot));

                        // Discard the first item
                        if (!hasRoom)
                        {
                            assert(!curItems.empty());
                            T* discardItem = curItems.front();

                            for (uint32_t sampleIdx = 0; sampleIdx < samples.size(); sampleIdx++)
                            {
                                if (samples[sampleIdx].first == typeid(*discardItem))
                                {
                                    if (itemInfo[sampleIdx].Count != Item::InfiniteItemCount)
                                    {
                                        itemInfo[sampleIdx].Count++;
                                    }
                                    equippedItems.erase(sampleIdx);
                                    updateCornerText(panel, font, sampleIdx, itemInfo[sampleIdx].Count, equippedItems);

                                    break;
                                }
                            }

                            selectedDwarf->Dwarf->DiscardItem(discardItem);
                            discardItem->Terminate();
                        }

                        T* item = selectedDwarf->Dwarf->GiveItem(itemInfo[clickedButton].Constructor);

                        // Turn off headlamps, they are too bright
                        if (IsA<Item::Headlamp>(item))
                        {
                            AsA<Item::Headlamp>(item)->SetLightEnabled(false);
                        }

                        if (itemInfo[clickedButton].Count != Item::InfiniteItemCount)
                        {
                            itemInfo[clickedButton].Count--;
                        }
                        equippedItems.insert(clickedButton);
                        updateCornerText(panel, font, clickedButton, itemInfo[clickedButton].Count, equippedItems);

                        playEquipSound(item);
                    }
                    else if (equipped && equippedItems.size() > 1)
                    {
                        std::vector<T*> curItems = selectedDwarf->Dwarf->GetItems<T>(slot);
                        for (T* curItem : curItems)
                        {
                            if (samples[clickedButton].first == typeid(*curItem))
                            {
                                selectedDwarf->Dwarf->DiscardItem(curItem);
                                curItem->Terminate();
                                break;
                            }
                        }

                        if (itemInfo[clickedButton].Count != Item::InfiniteItemCount)
                        {
                            itemInfo[clickedButton].Count++;
                        }
                        equippedItems.erase(clickedButton);
                        updateCornerText(panel, font, clickedButton, itemInfo[clickedButton].Count, equippedItems);
                    }
                }

                uint32_t mouseoverButton;
                if (panel->IsButtonMouseOver(mouseoverButton))
                {
                    curTooltip = samples[mouseoverButton].second;
                }
            }

            static void updateCornerText(ButtonPanel* panel, const Graphics::Font* font, uint32_t index, uint32_t count, std::set<uint32_t>& equippedItems)
            {
                std::string itemCountCornerText = "";
                if (count != Item::InfiniteItemCount)
                {
                    itemCountCornerText = Format("<color=white>%u</color>", count);
                }
                panel->SetButtonCornerText(index, Alignment_BottomRight, font, itemCountCornerText);

                std::string equippedItemCornerText = "";
                if (equippedItems.find(index) != equippedItems.end())
                {
                    equippedItemCornerText = Format("<color=lime>E</color>");
                }
                panel->SetButtonCornerText(index, Alignment_TopRight, font, equippedItemCornerText);
            }

            Audio::SoundManager* _soundManager = nullptr;

            Vector2f _position;
            float _skeletonScale = 1.0f;
            float _uiScale = 1.0f;

            Vector2f _mouseScreenSpace;

            bool _mouseOver;
            bool _clickingAccept;
            bool _clickingCancel;

            Input::InputBindCode _selectBind;

            ResourcePointer<HUD::ButtonPanel> _weaponPanel;
            std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>> _sampleWeapons;

            ResourcePointer<HUD::ButtonPanel> _trinketPanel;
            std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>> _sampleTrinkets;

            ResourcePointer<HUD::ButtonPanel> _armorPanel;
            std::vector<std::pair<std::type_index, ResourcePointer<const HUD::Panel>>> _sampleArmor;

            ResourcePointer<Animation::SkeletonInstance> _menuSkeleton;
            ResourcePointer<const Graphics::PolygonMaterialSet> _acceptHighlight;
            ResourcePointer<const Graphics::PolygonMaterialSet> _cancelHighlight;
            Lights::PolygonMaterialLight _acceptLight;
            Lights::PolygonMaterialLight _cancelLight;

            Audio::SoundSet _chainSounds;
            std::vector<Audio::SoundSet> _thudSounds;

            std::string _acceptText;
            ResourcePointer<const HUD::Panel> _acceptTooltip;
            std::string _cancelText;
            ResourcePointer<const HUD::Panel> _cancelTooltip;

            LoadoutDwarfInfo* _selectedDwarf;
            DwarfTypeState* _selectedDwarfTypeState;

            Vector2f _cursorSize;
            ResourcePointer<const HUD::Panel> _tooltip;

            ResourcePointer<const Graphics::Font> _hudFont;
        };

        static const std::string GoPanelSkeletonPath = "Skeletons/UI/Loadout/go_button.skel";
        static const std::string GoPanelMaterialPath = "Skeletons/UI/Loadout/go_button.polymatset";
        static const std::string GoPanelHighlightMaterialPath = "Skeletons/UI/Loadout/go_button_highlight.polymatset";

        static const std::string GoPanelButtonJoint = "origin";
        static const std::string GoPanelShowAnimation = "move_in";
        static const std::string GoPanelHideAnimation = "move_out";

        static const std::string GoPanelTooltipStringCode = "go_button_tooltip";

        class GoPanel : public Resource, public Graphics::IDrawable
        {
        public:
            GoPanel(Input::InputBindCode selectBind, const Localization::StringTable* strings, Audio::SoundManager* soundManager)
                : _soundManager(soundManager)
                , _position()
                , _mouseOver(false)
                , _clickingAccept(false)
                , _selectBind(selectBind)
                , _menuSkeleton(nullptr)
                , _hightlightMaterial(nullptr)
                , _acceptLight()
                , _tooltipText()
                , _tooltip(nullptr)
            {
                _tooltipText = strings->GetString(GoPanelTooltipStringCode);

                _chainSounds.AddSounds(ChainSoundPaths);
                for (const auto& thudSoundPaths : ThudSoundPaths)
                {
                    _thudSounds.push_back(Audio::SoundSet(thudSoundPaths));
                }
            }

            void SetPosition(const Vector2f& pos, float skeletonScale, float uiScale)
            {
                _position = pos;
                _skeletonScale = skeletonScale;
                _uiScale = uiScale;
                if (_menuSkeleton)
                {
                    _menuSkeleton->SetPosition(_position);
                    _menuSkeleton->SetScale(_skeletonScale);
                }
            }

            bool IsMouseOver() const
            {
                return _mouseOver;
            }

            bool IsJustClickingAccept() const
            {
                return _clickingAccept;
            }

            void Show()
            {
                if (_menuSkeleton && _menuSkeleton->GetCurrentAnimation() != GoPanelShowAnimation)
                {
                    _menuSkeleton->PlayAnimation(GoPanelShowAnimation, false, 0.15f);
                }
            }

            void Hide()
            {
                if (_menuSkeleton && _menuSkeleton->GetCurrentAnimation() != GoPanelHideAnimation)
                {
                    _menuSkeleton->PlayAnimation(GoPanelHideAnimation, false, 0.15f);
                }
            }

            const HUD::Panel* GetTooltip() const
            {
                return _tooltip;
            }

            void LoadContent(Content::ContentManager* contentManager)
            {
                _menuSkeleton = Content::CreateSkeletonInstance(contentManager, GoPanelSkeletonPath, GoPanelMaterialPath);
                _menuSkeleton->SetPosition(_position);
                _menuSkeleton->SetScale(_skeletonScale);
                _menuSkeleton->PlayAnimation(GoPanelHideAnimation, false, 0.0f, _menuSkeleton->GetAnimationLength(GoPanelHideAnimation));
                _hightlightMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(GoPanelHighlightMaterialPath);

                _chainSounds.LoadContent(contentManager);
                for (auto& thudSounds : _thudSounds)
                {
                    thudSounds.LoadContent(contentManager);
                }

                _acceptLight.Material = _menuSkeleton->GetMaterial(GoPanelButtonJoint);

                _tooltip = HUD::CreateSimpleTextTooltip(contentManager, _tooltipText);
            }

            void UnloadContent()
            {
                SafeRelease(_menuSkeleton);

                _chainSounds.UnloadContent();
                for (auto& thudSounds : _thudSounds)
                {
                    thudSounds.UnloadContent();
                }

                SafeRelease(_hightlightMaterial);
                SafeRelease(_tooltip);
            }

            void Update(const Input::FrameInput& input, const Camera& camera, double totalTime, float dt)
            {
                Vector2f mousePos = input.UnProjectMousePosition(camera);

                _menuSkeleton->Update(totalTime, dt);

                if (_menuSkeleton->HasAnimationTagJustPassed("chain"))
                {
                    _soundManager->PlaySinglePositionalSound(_chainSounds.GetNextSound(), Audio::SoundPriority::High, _menuSkeleton->GetJointPosition("chain_sound_source"), ChainSoundRadius.first, ChainSoundRadius.second, ChainSoundVolume);
                }
                if (_menuSkeleton->HasAnimationTagJustPassed("thud"))
                {
                    for (const auto& thudSound : _thudSounds)
                    {
                        _soundManager->PlaySinglePositionalSound(thudSound.GetNextSound(), Audio::SoundPriority::High, _menuSkeleton->GetJointPosition("thud_sound_source"), ChainSoundRadius.first, ChainSoundRadius.second, ChainSoundVolume);
                    }
                }

                const Animation::Transformation& acceptLightTransform = _menuSkeleton->GetJointTransformation(GoPanelButtonJoint);
                _acceptLight.Position = acceptLightTransform.Position;
                _acceptLight.Rotation = acceptLightTransform.Rotation;
                _acceptLight.Scale = acceptLightTransform.Scale.X;

                _mouseOver = _menuSkeleton->JointContainsPoint(GoPanelButtonJoint, mousePos);
                _clickingAccept = _mouseOver && input.IsBindJustPressed(_selectBind);

                _acceptLight.LightColor = Color::FromBytes(32, 64, 32, 255);
                _acceptLight.EmissiveColor = Color::FromBytes(64, 128, 64, 255);

                _menuSkeleton->ClearMaterials(_hightlightMaterial);
                if (_mouseOver)
                {
                    _menuSkeleton->MergeMaterialSet(_hightlightMaterial);
                }
                else
                {
                    float brightness = Clamp(Sine(totalTime * GoButtonHintLightStrobeFrequency), 0.0f, 1.0f);
                    _acceptLight.LightColor *= brightness;
                    _acceptLight.EmissiveColor *= brightness;
                }
            }

            const Rectanglef& GetDrawBounds() const override
            {
                return _menuSkeleton->GetDrawBounds();
            }

            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override
            {
                _menuSkeleton->Draw(spriteRenderer);
            }

            void Draw(Graphics::LevelRenderer* levelRenderer) const
            {
                levelRenderer->AddDrawable(this, false);
                levelRenderer->AddLight(_acceptLight);
            }

        protected:
            virtual ~GoPanel()
            {
            }

        private:
            Audio::SoundManager* _soundManager = nullptr;

            Vector2f _position;
            float _skeletonScale = 1.0f;
            float _uiScale = 1.0f;

            bool _mouseOver;
            bool _clickingAccept;

            Input::InputBindCode _selectBind;

            Animation::SkeletonInstance* _menuSkeleton;
            const Graphics::PolygonMaterialSet* _hightlightMaterial;
            Lights::PolygonMaterialLight _acceptLight;

            Audio::SoundSet _chainSounds;
            std::vector<Audio::SoundSet> _thudSounds;

            std::string _tooltipText;
            HUD::Panel* _tooltip;
        };
    }

    template <>
    void EnumeratePreloads<HUD::LoadoutPanel>(PreloadSet& preloads)
    {
        preloads.insert(HUD::LoadoutPanelSkeletonPath);
        preloads.insert(HUD::LoadoutPanelMaterialPath);
        preloads.insert(HUD::LoadoutPanelAcceptHighlightMaterialPath);
        preloads.insert(HUD::LoadoutPanelCancelHighlightMaterialPath);

        for (const auto& thudSound : HUD::ThudSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, thudSound);
        }
        EnumerateSoundVectorPreloads(preloads, HUD::ChainSoundPaths);
    }

    template <>
    void EnumeratePreloads<HUD::GoPanel>(PreloadSet& preloads)
    {
        preloads.insert(HUD::GoPanelSkeletonPath);
        preloads.insert(HUD::GoPanelMaterialPath);
        preloads.insert(HUD::GoPanelHighlightMaterialPath);

        for (const auto& thudSound : HUD::ThudSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, thudSound);
        }
        EnumerateSoundVectorPreloads(preloads, HUD::ChainSoundPaths);
    }

    namespace Character
    {
        static const std::string PortraitSkeletonPath = "Skeletons/UI/Portraits/Portrait.skel";
        static const std::string BrewerPortraitMaterialPath = "Skeletons/UI/Portraits/brewer_p.polymatset";
        static const std::string BuilderPortraitMaterialPath = "Skeletons/UI/Portraits/builder_p.polymatset";
        static const std::string CookPortraitMaterialPath = "Skeletons/UI/Portraits/cook_p.polymatset";
        static const std::string FighterPortraitMaterialPath = "Skeletons/UI/Portraits/fighter_p.polymatset";
        static const std::string MinerPortraitMaterialPath = "Skeletons/UI/Portraits/miner_p.polymatset";
        static const std::string NavigatorPortraitMaterialPath = "Skeletons/UI/Portraits/navigator_p.polymatset";
        static const std::string PortraitLightJointName = "joint_0";

        static const std::string ChalkboardSkeletonPath = "Skeletons/UI/Chalkboard/chalkboard.skel";
        static const std::string ChalkboardMaterialPath = "Skeletons/UI/Chalkboard/chalkboard.polymatset";
        static const std::string ChalkboardChalkManAttachJoint = "chalk_man_%u";

        static const std::string ChalkManSkeletonPath = "Skeletons/UI/Chalkboard/chalk_man.skel";
        static const std::vector<std::string> ChalkManMaterialPaths
        {
            "Skeletons/UI/Chalkboard/chalk_man1.polymatset",
            "Skeletons/UI/Chalkboard/chalk_man2.polymatset",
            "Skeletons/UI/Chalkboard/chalk_man3.polymatset",
        };
        static const std::vector<std::string> ChalkManCrossMaterialPaths
        {
            "Skeletons/UI/Chalkboard/chalk_cross1.polymatset",
            "Skeletons/UI/Chalkboard/chalk_cross2.polymatset",
            "Skeletons/UI/Chalkboard/chalk_cross3.polymatset",
        };
        static const std::string ChalkManCrossAnimation = "cross";
        static const std::string ChalkManUncrossAnimation = "uncross";

        static const std::string HUDMaterialSetPath = "HUD/hud.hudmatset";
        static const std::string HUDFontPath = "Fonts/hud_font.ttf";

        static const std::map<std::type_index, std::string> DwarfTypeNameStrings =
        {
            { typeid(BrewerDwarf), "brewer_type_name" },
            { typeid(BuilderDwarf), "builder_type_name" },
            { typeid(CookDwarf), "cook_type_name" },
            { typeid(FighterDwarf), "fighter_type_name" },
            { typeid(MinerDwarf), "miner_type_name" },
            { typeid(NavigatorDwarf), "navigator_type_name" },
        };
        static const std::string GenericDwarfTypeNameString = "generic_type_name";

        static const Color SelectedCharacterHighlightColor = Color::FromFloats(0.75f, 0.75f, 0.75f, 1.0f);
        static const Color InteractiveCharacterHighlightColor = Color::FromFloats(0.75f, 0.75f, 0.1f, 1.0f);

        class LoadoutController : public Controller
        {
        public:
            LoadoutController(const ControllerParameters& parameters, const Level::CampaignLevelInfo& levelInfo, LevelChangeFunction levelChangeFunc,
                              PreloadLevelFunction preloadLevelFunc,  ExitGameFunction exitFunc, TransitionToCampaignMenuFunction menuFunc,
                              PauseGameFunction pauseFunc, SaveProfileFunction saveProfileFunc, Level::CampaignLevelConstructor<> campaignLevel)
                : Controller(parameters)

                , _dwarfTypes()
                , _levelInfo(levelInfo)

                , _genericTypeTooltip(nullptr)

                , _spawnPos()
                , _readyPosLeft()
                , _readyPosRight()
                , _chestPos()
                , _menuPos()
                , _portraitPos()
                , _chalkboardPos()

                , _createdADwarf(false)
                , _showedCreateDwarfTutorial(false)
                , _totalLevelTime(0.0f)

                , _loadoutPanel(nullptr)
                , _loadoutPanelIn(false)

                , _goPanel(nullptr)
                , _goPanelIn(false)

                , _hudSelectBind(Input::InputBindCode_Invalid)

                , _selectedDwarf()

                , _dwarves()
                , _removalPendingDwarves()

                , _chalkboard(nullptr)
                , _firstGenericChalkman(0)
                , _chalkmen()

                , _curTooltip(nullptr)

                , _mousePosScreen()
                , _curViewport()

                , _hudMatSet(nullptr)
                , _cursor(nullptr)
                , _hudFont(nullptr)

                , _levelChangeFunc(levelChangeFunc)
                , _startedPreload(false)
                , _preloadLevelFunc(preloadLevelFunc)
                , _exitFunc(exitFunc)
                , _menuFunc(menuFunc)
                , _pauseFunc(pauseFunc)
                , _saveProfileFunc(saveProfileFunc)
                , _campaignLevel(campaignLevel)

                , _profile(nullptr)
            {
                const Settings::Profile* profile = GetLevel()->GetProfile();
                const Localization::StringTable* strings = GetLevel()->GetStringTable();

                _hudSelectBind = profile->GetBindCode("hud_select");

                _loadoutPanel = new HUD::LoadoutPanel(_hudSelectBind, strings, GetLevel()->GetSoundManager());
                _goPanel = new HUD::GoPanel(_hudSelectBind, strings, GetLevel()->GetSoundManager());

                _profile = GetLevel()->GetProfile<Settings::TheDeepDeepProfile>();

                initializeDwarfTypes();
            }

        protected:
            virtual ~LoadoutController()
            {
                SafeRelease(_loadoutPanel);
                SafeRelease(_goPanel);
            }

            virtual void OnCreate() override
            {
                initializeTriggerPositions();

                //_loadoutPanel->SetPosition(_menuPos);
                animateLoadoutOut();

                //_goPanel->SetPosition(_goPos);
                animateGoPanelOut();

                _chalkboard->SetPosition(_chalkboardPos);
                for (uint32_t i = 0; i < _chalkmen.size(); i++)
                {
                    std::string attachJointName = Format(ChalkboardChalkManAttachJoint.c_str(), i);
                    _chalkmen[i]->SetPosition(_chalkboard->GetJointPosition(attachJointName));
                }

                assert(_portraitPos.size() >= _dwarfTypes.size());
                uint32_t chalkmanIndex = 0;
                uint32_t portraitLocationIndex = 0;
                for (DwarfTypeMap::iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    dwarfTypeInfo &typeInfo = i->second;

                    typeInfo.Portrait->SetPosition(_portraitPos[portraitLocationIndex++]);

                    const Animation::Transformation& lightPortraitTransform = typeInfo.Portrait->GetJointTransformation(PortraitLightJointName);
                    typeInfo.PortraitLight.Position = lightPortraitTransform.Position;
                    typeInfo.PortraitLight.Rotation = lightPortraitTransform.Rotation;
                    typeInfo.PortraitLight.Scale = lightPortraitTransform.Scale.X;
                    typeInfo.PortraitLight.LightColor = Color::FromBytes(64, 64, 64, 255);
                    typeInfo.PortraitLight.EmissiveColor = Color::FromBytes(128, 128, 128, 255);

                    if (typeInfo.MinCharacters > 0)
                    {
                        typeInfo.FirstChalkMan = chalkmanIndex;

                        uint32_t end = chalkmanIndex + typeInfo.MinCharacters;
                        assert(end <= _chalkmen.size());
                        while (chalkmanIndex < end)
                        {
                            _chalkmen[chalkmanIndex]->SetJointColor("cman", typeInfo.HintColor);
                            chalkmanIndex++;
                        }
                    }
                }
                _firstGenericChalkman = chalkmanIndex;
            }

            virtual void OnDestroy() override
            {
                SafeRelease(_curTooltip);
            }

            virtual void OnLoadContent(Content::ContentManager* contentManager) override
            {
                _hudMatSet = contentManager->Load<Graphics::HUDMaterialSet>(HUDMaterialSetPath);

                _cursor = HUD::CreateCursor(contentManager, "HUD/hud.cursorset", "cursor_regular");

                _hudFont = contentManager->Load<Graphics::Font>(HUDFontPath);

                _loadoutPanel->LoadContent(contentManager);
                _loadoutPanel->SetCursorSize(Vector2f(_cursor->Width(), _cursor->Height()));

                _goPanel->LoadContent(contentManager);

                const Animation::Skeleton* portraitSkeleton = contentManager->Load<Animation::Skeleton>(PortraitSkeletonPath);
                for (DwarfTypeMap::iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    const Graphics::PolygonMaterialSet* portraitMaterialSet = contentManager->Load<Graphics::PolygonMaterialSet>(i->second.PortraitMaterialPath);
                    i->second.Portrait = new Animation::SkeletonInstance(portraitSkeleton, portraitMaterialSet);
                    i->second.PortraitLight.Material = i->second.Portrait->GetMaterial(PortraitLightJointName);
                    SafeRelease(portraitMaterialSet);

                    std::string nameCode = DwarfTypeNameStrings.at(i->first);
                    std::string typeName = GetLevel()->GetStringTable()->GetString(nameCode);

                    i->second.Tooltip = HUD::CreateSimpleTextTooltip(contentManager, typeName);
                }

                std::string genericTypeName = GetLevel()->GetStringTable()->GetString(GenericDwarfTypeNameString);
                _genericTypeTooltip = HUD::CreateSimpleTextTooltip(contentManager, genericTypeName);

                SafeRelease(portraitSkeleton);

                const Animation::Skeleton* chalkboardSkeleton = contentManager->Load<Animation::Skeleton>(ChalkboardSkeletonPath);

                // Count the chalk men
                uint32_t chalkMenCount = 0;
                while (chalkboardSkeleton->HasJoint(Format(ChalkboardChalkManAttachJoint.c_str(), chalkMenCount)))
                {
                    chalkMenCount++;
                }
                chalkMenCount = Min(_levelInfo.MaxTotalCharacters, chalkMenCount);

                const Graphics::PolygonMaterialSet* chalkboardMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(ChalkboardMaterialPath);
                _chalkboard = new Animation::SkeletonInstance(chalkboardSkeleton, chalkboardMaterial);
                SafeRelease(chalkboardSkeleton);
                SafeRelease(chalkboardMaterial);

                const Animation::Skeleton* chalkmanSkeleton = contentManager->Load<Animation::Skeleton>(ChalkManSkeletonPath);
                for (uint32_t i = 0; i < chalkMenCount; i++)
                {
                    const Graphics::PolygonMaterialSet* chalkmanMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(Random::RandomItem(ChalkManMaterialPaths));
                    const Graphics::PolygonMaterialSet* chalkmanCrossMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(Random::RandomItem(ChalkManCrossMaterialPaths));

                    Animation::SkeletonInstance* chalkMan = new Animation::SkeletonInstance(chalkmanSkeleton);
                    chalkMan->MergeMaterialSet(chalkmanMaterial);
                    chalkMan->MergeMaterialSet(chalkmanCrossMaterial);
                    chalkMan->PlayAnimation(ChalkManUncrossAnimation, false, 0.0f);

                    _chalkmen.push_back(chalkMan);

                    SafeRelease(chalkmanMaterial);
                    SafeRelease(chalkmanCrossMaterial);
                }
                SafeRelease(chalkmanSkeleton);

                _tutorialDisplay = new HUD::TutorialDisplay(contentManager, GetLevel()->GetStringTable(), GetLevel()->GetSoundManager(), _hudSelectBind, _profile);
                _tutorialDisplay->AnimateIn(true);
            }

            virtual void OnUnloadContent() override
            {
                _loadoutPanel->UnloadContent();
                _goPanel->UnloadContent();

                for (DwarfTypeMap::iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    i->second.PortraitLight.Material = nullptr;
                    SafeRelease(i->second.Portrait);
                    SafeRelease(i->second.Tooltip);
                }
                SafeRelease(_genericTypeTooltip);

                SafeRelease(_chalkboard);
                SafeReleaseAndClearContainer(_chalkmen);

                SafeRelease(_hudMatSet);
                SafeRelease(_cursor);
                SafeRelease(_hudFont);

                SafeRelease(_curTooltip);

                SafeDelete(_tutorialDisplay);
            }

            virtual void OnUpdate(const Input::FrameInput& input, double totalTime, float dt) override
            {
                if (!_startedPreload)
                {
                    //_preloadLevelFunc(_campaignLevel(Level::CampaignLevelParameters()));
                    _startedPreload = true;
                }

                Level::LevelInstance* lvl = GetLevel();
                Level::CameraController& cameraController = lvl->GetCameraController();
                const Camera& camera = cameraController.GetCamera();

                const Camera hudCamera = lvl->GetHUDCamera();
                const Rectanglef& viewport = hudCamera.GetViewport();

                Vector2f mousePosHUD = input.UnProjectMousePosition(hudCamera);
                Vector2f mousePosWorld = input.UnProjectMousePosition(camera);
                _mousePosScreen = mousePosHUD;

                float uiScale = _profile->GetUIScale();

                if (viewport != _curViewport || uiScale != _uiScale)
                {
                    _curViewport = viewport;
                    _uiScale = uiScale;
                }

                SafeRelease(_curTooltip);

                float cameraZoom = cameraController.GetCamera().GetZoom();

                // To get a 1.0 scale at 1080p
                float panelScale = cameraZoom * 2.43277435558f;

                // Update the loadout panel
                _loadoutPanel->SetPosition(camera.Project(_menuPos), panelScale, _uiScale);
                _loadoutPanel->Update(input, hudCamera, totalTime, dt);
                bool mouseOnLoadout = _loadoutPanel->IsMouseOver();
                if (mouseOnLoadout)
                {
                    _curTooltip = _loadoutPanel->GetTooltip();
                }

                _goPanel->SetPosition(camera.Project(_goPos), panelScale, _uiScale);
                _goPanel->Update(input, hudCamera, totalTime, dt);
                bool mouseOnGo = _goPanel->IsMouseOver();
                if (mouseOnGo)
                {
                    _curTooltip = _goPanel->GetTooltip();
                }

                _tutorialDisplay->Update(totalTime, dt, input, hudCamera);

                // Remove dwarves that are pending removal and have reached the exit
                for (int32_t i = 0; i < int32_t(_removalPendingDwarves.size()); i++)
                {
                    const Vector2f& dwarfPos = _removalPendingDwarves[i]->Dwarf->GetPosition();
                    if (dwarfPos.X < _spawnPos.X)
                    {
                        removeDwarfItems(_removalPendingDwarves[i]);

                        SafeDelete(_removalPendingDwarves[i]);
                        _removalPendingDwarves.erase(_removalPendingDwarves.begin() + i);
                        i--;
                    }
                }

                // Check if any portraits are being clicked
                bool mouseOnPortrait = false;
                for (DwarfTypeMap::iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    dwarfTypeInfo& typeInfo = i->second;

                    bool canCreate = canCreateDwarfType(i->first);
                    bool mouseOver = typeInfo.Portrait->ContainsPoint(mousePosWorld);

                    if (mouseOver)
                    {
                        mouseOnPortrait = true;
                        _curTooltip = typeInfo.Tooltip;

                        if (input.IsBindJustPressed(_hudSelectBind))
                        {
                            // Create a new dwarf of this type
                            createDwarf(i->first);
                        }
                    }

                    if (canCreate)
                    {
                        typeInfo.Portrait->SetColor(Color::White);
                        typeInfo.PortraitLight.LightColor = Color::FromBytes(64, 64, 64, 255);
                        typeInfo.PortraitLight.EmissiveColor = Color::FromBytes(64, 64, 64, 255);

                        if (!mouseOver)
                        {
                            float brightness = Clamp(Sine(totalTime * PortraitHintLightStrobeFrequency), 0.0f, 1.0f);
                            typeInfo.PortraitLight.LightColor *= brightness;
                            typeInfo.PortraitLight.EmissiveColor *= brightness;
                        }
                    }
                    else
                    {
                        typeInfo.Portrait->SetColor(Color::Gray);
                        typeInfo.PortraitLight.LightColor = Color::FromBytes(0, 0, 0, 255);
                        typeInfo.PortraitLight.EmissiveColor = Color::FromBytes(0, 0, 0, 255);
                    }
                }

                // Check if any of the chalk men are moused over
                for (uint32_t i = 0; i < _chalkmen.size(); i++)
                {
                    if (_chalkmen[i]->ContainsPoint(mousePosWorld))
                    {
                        std::type_index chalkManType = getChalkManType(i);
                        auto iter = _dwarfTypes.find(chalkManType);
                        if (iter != _dwarfTypes.end())
                        {
                            _curTooltip = iter->second.Tooltip;
                        }
                        else
                        {
                            _curTooltip = _genericTypeTooltip;
                        }
                    }
                }

                // Check if any dwarves have been clicked
                bool mouseOnDwarf = false;
                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    if (_dwarves[i]->Dwarf->Intersects(mousePosWorld))
                    {
                        mouseOnDwarf = true;
                        if (input.IsBindJustPressed(_hudSelectBind))
                        {
                            selectDwarf(_dwarves[i]);
                            break;
                        }
                    }
                }

                // Select nothing if a click occurs on nothing
                if (input.IsBindJustPressed(_hudSelectBind) && !mouseOnDwarf && !mouseOnPortrait && !mouseOnLoadout && !mouseOnGo)
                {
                    selectDwarf(nullptr);
                }

                if (_loadoutPanel->IsJustClickingAccept())
                {
                    selectDwarf(nullptr);
                }
                if (_loadoutPanel->IsJustClickingCancel() && _selectedDwarf != nullptr)
                {
                    removeDwarf(_selectedDwarf);
                }

                _totalLevelTime += dt;
                if (_totalLevelTime > ShowCreateDwarfTutorialStartTime && !_showedCreateDwarfTutorial && !_createdADwarf &&
                    !_tutorialDisplay->HasShownTutorial(TutorialType::LoadoutClickPortrait))
                {
                    _showedCreateDwarfTutorial = true;

                    auto firstDwarfType = _dwarfTypes.begin();
                    const Rectanglef& portraitBounds = firstDwarfType->second.Portrait->GetDrawBounds();
                    Vector2f tooltipPos = portraitBounds.Middle() - Vector2f(portraitBounds.W * 0.5f, 0.0f);
                    Vector2f portraitPosHUD = hudCamera.UnProject(camera.Project(tooltipPos));
                    _tutorialDisplay->ShowTutorial(TutorialType::LoadoutClickPortrait, portraitPosHUD);
                    _profile->MarkTutorialShown(TutorialType::LoadoutClickPortrait);
                }
                else if (_createdADwarf && _showedCreateDwarfTutorial)
                {
                    _tutorialDisplay->StopShowingTutorial(TutorialType::LoadoutClickPortrait);
                }

                for (DwarfTypeMap::iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    i->second.Portrait->Update(totalTime, dt);
                }

                _chalkboard->Update(totalTime, dt);
                for (uint32_t i = 0; i < _chalkmen.size(); i++)
                {
                    _chalkmen[i]->Update(totalTime, dt);
                }

                // Update the dwarves
                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    const float equipDistance = 60.0f;
                    if (!_dwarves[i]->IsEquiped && (_dwarves[i]->Dwarf->GetPosition().X - _chestPos.X) <= equipDistance)
                    {
                        equipDwarfDefaultItems(_dwarves[i]);
                    }

                    if (_selectedDwarf && _dwarves[i]->Dwarf == _selectedDwarf->Dwarf)
                    {
                        bool nearChest = Abs(_chestPos.X - _dwarves[i]->Dwarf->GetPosition().X) <= equipDistance;
                        if (nearChest && !_loadoutPanelIn)
                        {
                            assert(_dwarves[i]->IsEquiped);
                            animateLoadoutIn(_selectedDwarf);
                        }
                        else if (!nearChest && _loadoutPanelIn)
                        {
                            animateLoadoutOut();
                        }

                        _dwarves[i]->Dwarf->SetHighlightColor(SelectedCharacterHighlightColor);
                    }
                    else
                    {
                        _dwarves[i]->Dwarf->SetHighlightColor(Color::Transparent);
                    }
                }

                bool goPanelShouldBeIn = _dwarves.size() >= _levelInfo.MaxTotalCharacters && _selectedDwarf == nullptr;
                if (goPanelShouldBeIn && !_goPanelIn)
                {
                    animateGoPanelIn();
                }
                else if (!goPanelShouldBeIn && _goPanelIn)
                {
                    animateGoPanelOut();
                }

                // User wants to go to the level
                if (_goPanelIn && _goPanel->IsJustClickingAccept())
                {
                    // Equip all unequiped dwarves
                    for (uint32_t i = 0; i < _dwarves.size(); i++)
                    {
                        if (!_dwarves[i]->IsEquiped)
                        {
                            equipDwarfDefaultItems(_dwarves[i]);
                        }
                    }

                    _levelChangeFunc(_campaignLevel(generateCampaignLevelParameters()));
                }

                SafeAddRef(_curTooltip);
            }

            virtual void OnDraw(Level::LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override
            {
                if (layer->IsPrimary())
                {
                    for (DwarfTypeMap::const_iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                    {
                        levelRenderer->AddDrawable(i->second.Portrait, false);
                        levelRenderer->AddLight(i->second.PortraitLight);
                    }

                    levelRenderer->AddDrawable(_chalkboard, false);
                    for (uint32_t i = 0; i < _chalkmen.size(); i++)
                    {
                        levelRenderer->AddDrawable(_chalkmen[i], false);
                    }

                    //_loadoutPanel->Draw(levelRenderer);

                    //_goPanel->Draw(levelRenderer);

                    levelRenderer->SetCursor(_cursor);
                }
            }

            virtual void OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const override
            {
                // Draw the loadout panel
                //_loadoutPanel->Draw(spriteRenderer);

                _loadoutPanel->Draw(spriteRenderer);
                _goPanel->Draw(spriteRenderer);

                if (_curTooltip)
                {
                    Vector2f minimumSize = _curTooltip->GetMinimumSize(_uiScale);
                    Rectanglef tooltipBounds(_mousePosScreen + Vector2f(0.0f, _cursor->Height() + 3.0f), minimumSize);
                    _curTooltip->Draw(spriteRenderer, Color::White, tooltipBounds, _uiScale);
                }

                _tutorialDisplay->Draw(spriteRenderer);
            }

        private:
            static const std::string& dwarfTypeToKey(const std::type_index& type)
            {
                static const std::map<std::type_index, std::string> typeToKeyMap
                {
                    { typeid(BrewerDwarf), Dwarves::Brewer() },
                    { typeid(BuilderDwarf), Dwarves::Builder() },
                    { typeid(CookDwarf), Dwarves::Cook() },
                    { typeid(FighterDwarf), Dwarves::Fighter() },
                    { typeid(MinerDwarf), Dwarves::Miner() },
                    { typeid(NavigatorDwarf), Dwarves::Navigator() },
                };
                assert(typeToKeyMap.find(type) != typeToKeyMap.end());
                return typeToKeyMap.at(type);
            }

            static void restartAtCheckpoint(LevelChangeFunction levelChangeFunc, Level::CampaignLevelConstructor<> campaignLevel,
                                            Level::CampaignLevelParameters params, Settings::TheDeepDeepProfile* profile, 
                                            std::shared_ptr<const Level::CampaignLevelCheckpoint> checkpoint)
            {
                params.Checkpoint = checkpoint;

                using namespace std::placeholders;
                params.CheckpointFunction = std::bind(&LoadoutController::restartAtCheckpoint, levelChangeFunc, campaignLevel, params, profile, _1);

                GameState::LevelConstructor<> constructor = campaignLevel(params);

                profile->EnumerateItemPreloads(constructor.Preloads);
                profile->EnumerateAbilityPreloads(constructor.Preloads);

                levelChangeFunc(constructor);
            }

            Level::CampaignLevelParameters generateCampaignLevelParameters() const
            {
                Level::CampaignLevelParameters params = _generateCampaignLevelParametersInternal();

                Level::CampaignLevelParameters paramsCopy = params;
                LevelChangeFunction levelChangeFuncCopy = _levelChangeFunc;
                Level::CampaignLevelConstructor<> campaignLevelCopy = _campaignLevel;
                Settings::TheDeepDeepProfile* profile = GetLevel()->GetProfile<Settings::TheDeepDeepProfile>();

                using namespace std::placeholders;
                params.CheckpointFunction = std::bind(&LoadoutController::restartAtCheckpoint, levelChangeFuncCopy, campaignLevelCopy, paramsCopy, profile, _1);

                return params;
            }

            Level::CampaignLevelParameters _generateCampaignLevelParametersInternal() const
            {
                Level::CampaignLevelParameters params;

                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    std::type_index type = typeid(*_dwarves[i]->Dwarf);
                    const dwarfTypeInfo &typeInfo = _dwarfTypes.at(type);

                    Level::CampaignLevelCharacterInfo characterInfo;
                    characterInfo.Constructor = typeInfo.Constructor(_dwarves[i]->Dwarf->GetInfo());

                    for (auto weaponIndex : _dwarves[i]->WeaponIndex)
                    {
                        characterInfo.Items.push_back(typeInfo.TypeState.Weapons[weaponIndex].Constructor);
                    }
                    for (auto armorIndex : _dwarves[i]->ArmorIndex)
                    {
                        characterInfo.Items.push_back(typeInfo.TypeState.Armors[armorIndex].Constructor);
                    }
                    for (auto trinketIndex : _dwarves[i]->TrinketIndex)
                    {
                        characterInfo.Items.push_back(typeInfo.TypeState.Trinkets[trinketIndex].Constructor);
                    }

                    uint32_t abilityCount = _profile->GetAbilityCount(dwarfTypeToKey(type));
                    for (uint32_t j = 0; j < abilityCount; j++)
                    {
                        characterInfo.Abilities.push_back(_profile->GetAbilityInfo(dwarfTypeToKey(type), j).Constructor);
                    }

                    params.Characters.push_back(characterInfo);
                }

                params.StartingResources = _levelInfo.StartingResources;

                params.Checkpoint = nullptr;

                params.ExitFunction = _exitFunc;
                params.MenuFunction = _menuFunc;
                params.PauseFunction = _pauseFunc;
                params.SaveProfile = _saveProfileFunc;

                Level::CampaignLevelInfo levelInfoCopy = _levelInfo;
                LevelChangeFunction levelChangeFuncCopy = _levelChangeFunc;
                PreloadLevelFunction preloadLevelFuncCopy = _preloadLevelFunc;
                TransitionToCampaignMenuFunction menuFuncCopy = _menuFunc;
                ExitGameFunction exitFuncCopy = _exitFunc;
                PauseGameFunction pauseFuncCopy = _pauseFunc;
                SaveProfileFunction saveProfileFuncCopy = _saveProfileFunc;
                Level::CampaignLevelConstructor<> campaignLevelCopy = _campaignLevel;
                Settings::TheDeepDeepProfile* profile = GetLevel()->GetProfile<Settings::TheDeepDeepProfile>();
                params.LoadoutFunction = [=]()
                {
                    GameState::LevelConstructor<Level::LoadoutLevel> constructor =
                        GameState::BindLevelConstructor<Level::LoadoutLevel>("Levels/loadout.lvl", levelInfoCopy, levelChangeFuncCopy, preloadLevelFuncCopy,
                            exitFuncCopy, menuFuncCopy, pauseFuncCopy, saveProfileFuncCopy, campaignLevelCopy);

                    profile->EnumerateItemPreloads(constructor.Preloads);
                    profile->EnumerateAbilityPreloads(constructor.Preloads);

                    levelChangeFuncCopy(constructor);
                };

                return params;
            }

            void setChalkManCrossed(uint32_t index, bool crossed)
            {
                assert(index < _chalkmen.size());
                _chalkmen[index]->PlayAnimation(crossed ? ChalkManCrossAnimation : ChalkManUncrossAnimation, false, 0.0f);
            }

            uint32_t getAssociatedChalkman(const std::type_info& type) const
            {
                uint32_t typeCount = getDwarfCount(type);
                assert(typeCount > 0);

                const dwarfTypeInfo& typeInfo = _dwarfTypes.at(type);

                if (typeCount <= typeInfo.MinCharacters)
                {
                    return typeInfo.FirstChalkMan + typeCount - 1;
                }
                else
                {
                    uint32_t genericCount = 0;
                    uint32_t nonGenericCount = 0;
                    for (DwarfTypeMap::const_iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                    {
                        uint32_t iterTypeCount = getDwarfCount(i->first);
                        const dwarfTypeInfo &iterTypeInfo = i->second;

                        if (iterTypeCount > iterTypeInfo.MinCharacters)
                        {
                            nonGenericCount += (iterTypeCount - iterTypeInfo.MinCharacters);
                        }

                        genericCount += iterTypeInfo.MinCharacters;
                    }

                    return genericCount + nonGenericCount - 1;
                }
            }

            std::type_index getChalkManType(uint32_t index) const
            {
                for (DwarfTypeMap::const_iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    const dwarfTypeInfo &typeInfo = i->second;
                    if (index >= typeInfo.FirstChalkMan && index < typeInfo.FirstChalkMan + typeInfo.MinCharacters)
                    {
                        return i->first;
                    }
                }

                return typeid(nullptr);
            }

            uint32_t getMissingRequiredDwarves() const
            {
                uint32_t missingCount = 0;
                for (DwarfTypeMap::const_iterator i = _dwarfTypes.begin(); i != _dwarfTypes.end(); i++)
                {
                    uint32_t iterTypeCount = getDwarfCount(i->first);
                    const dwarfTypeInfo &iterTypeInfo = i->second;

                    if (iterTypeCount < iterTypeInfo.MinCharacters)
                    {
                        missingCount += (iterTypeInfo.MinCharacters - iterTypeCount);
                    }
                }

                return missingCount;
            }

            uint32_t getDwarfCount(const std::type_index& type) const
            {
                uint32_t count = 0;
                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    if (std::type_index(typeid(*_dwarves[i]->Dwarf)) == type)
                    {
                        count++;
                    }
                }
                return count;
            }

            bool canCreateDwarfType(const std::type_index& type) const
            {
                const dwarfTypeInfo &dwarfTypeInfo = _dwarfTypes.at(type);
                uint32_t typeCount = getDwarfCount(type);
                uint32_t missingDwarves = getMissingRequiredDwarves();
                uint32_t remainingSlots = _levelInfo.MaxTotalCharacters - _dwarves.size();
                if (remainingSlots == 0 || (typeCount >= dwarfTypeInfo.MinCharacters && remainingSlots <= missingDwarves))
                {
                    // No way to create this dwarf
                    return false;
                }

                return true;
            }

            void animateLoadoutIn(LoadoutDwarfInfo* info)
            {
                assert(info);
                _loadoutPanel->SetSelectedDwarf(_selectedDwarf, &_dwarfTypes.at(typeid(*info->Dwarf)).TypeState);
                _loadoutPanel->Show();
                _loadoutPanelIn = true;
            }

            void animateLoadoutOut()
            {
                _loadoutPanel->UnsetSelectedDwarf();
                _loadoutPanel->Hide();
                _loadoutPanelIn = false;
            }

            void animateGoPanelIn()
            {
                _goPanel->Show();
                _goPanelIn = true;
            }

            void animateGoPanelOut()
            {
                _goPanel->Hide();
                _goPanelIn = false;
            }

            LoadoutDwarfInfo* createDwarf(const std::type_index& type)
            {
                if (!canCreateDwarfType(type))
                {
                    // No way to create this dwarf
                    return nullptr;
                }

                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance *primaryLayer = level->GetPrimaryLayer();
                dwarfTypeInfo &dwarfTypeInfo = _dwarfTypes[type];

                uint32_t typeCount = getDwarfCount(type);
                while (typeCount >= _profile->GetCharacterInfoCount(dwarfTypeToKey(type)))
                {
                    _profile->GenerateNewDwarfInfo(dwarfTypeToKey(type));
                }

                // Find an unused character profile
                uint32_t profileIdx = 0;
                while (dwarfTypeInfo.CheckedOutCharacterInfos.count(profileIdx) > 0)
                {
                    profileIdx++;
                }

                assert(profileIdx < _profile->GetCharacterInfoCount(dwarfTypeToKey(type)));
                dwarfTypeInfo.CheckedOutCharacterInfos.insert(profileIdx);

                const DwarfInfo& spawnDwarfInfo = _profile->GetCharacterInfo(dwarfTypeToKey(type), profileIdx);

                LoadoutDwarfInfo* dwarfInfo = new LoadoutDwarfInfo(
                    primaryLayer->SpawnCharacter(_spawnPos, spawnDwarfInfo.Name, this, dwarfTypeInfo.Constructor(spawnDwarfInfo)),
                    profileIdx);
                _dwarves.push_back(dwarfInfo);

                selectDwarf(dwarfInfo);

                setChalkManCrossed(getAssociatedChalkman(typeid(*dwarfInfo->Dwarf)), true);

                _createdADwarf = true;
                return dwarfInfo;
            }

            template <typename T>
            static void reclaimDwarfItemType(std::set<uint32_t>& equipedItemIndices, std::vector<Item::ItemInfo<T>>& items)
            {
                for (auto equipedItemIndex : equipedItemIndices)
                {
                    if (items[equipedItemIndex].Count != Item::InfiniteItemCount)
                    {
                        items[equipedItemIndex].Count++;
                    }
                }
                equipedItemIndices.clear();
            }

            void removeDwarfItems(LoadoutDwarfInfo* info)
            {
                assert(info->Dwarf);
                DwarfTypeState &typeInfo = _dwarfTypes[typeid(*info->Dwarf)].TypeState;

                reclaimDwarfItemType(info->WeaponIndex, typeInfo.Weapons);
                reclaimDwarfItemType(info->TrinketIndex, typeInfo.Trinkets);
                reclaimDwarfItemType(info->ArmorIndex, typeInfo.Armors);

                for (auto item : info->Dwarf->DiscardItems())
                {
                    item->Destroy();
                }

                info->IsEquiped = false;
            }

            void removeDwarf(LoadoutDwarfInfo* info)
            {
                assert(info);

                const std::type_info& type = typeid(*info->Dwarf);
                setChalkManCrossed(getAssociatedChalkman(type), false);

                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    if (_dwarves[i]->Dwarf == info->Dwarf)
                    {
                        _dwarves.erase(_dwarves.begin() + i);
                        break;
                    }
                }

                dwarfTypeInfo &dwarfTypeInfo = _dwarfTypes[type];
                assert(dwarfTypeInfo.CheckedOutCharacterInfos.count(info->ProfileIndex) > 0);
                dwarfTypeInfo.CheckedOutCharacterInfos.erase(info->ProfileIndex);

                _removalPendingDwarves.push_back(info);
                info->Dwarf->PushAction(CreateMoveAction(_removePos), false);
                info->Dwarf->PushAction(CreateDeathAction(), true);

                if (_selectedDwarf && info->Dwarf == _selectedDwarf->Dwarf)
                {
                    selectDwarf(nullptr);
                }
                else
                {
                    repositionReadyDwarves();
                }
            }

            template <typename T>
            static void giveFirstInfiniteItem(Dwarf* dwarf, Item::ItemSlot slot, std::set<uint32_t>& equippedItemIndex, const std::vector<Item::ItemInfo<T>>& items)
            {
                uint32_t itemsGiven = 0;
                for (uint32_t i = 0; i < items.size() && itemsGiven < dwarf->GetItemSlotCount(slot); i++)
                {
                    if (items[i].Count == Item::InfiniteItemCount)
                    {
                        T* item = dwarf->GiveItem(items[i].Constructor);

                        // Turn off headlamps, they are too bright
                        if (IsA<Item::Headlamp>(item))
                        {
                            AsA<Item::Headlamp>(item)->SetLightEnabled(false);
                        }

                        equippedItemIndex.insert(i);

                        itemsGiven++;
                    }
                }
            }

            void equipDwarfDefaultItems(LoadoutDwarfInfo* info)
            {
                assert(info != nullptr);
                const DwarfTypeState &typeInfo = _dwarfTypes[typeid(*info->Dwarf)].TypeState;
                giveFirstInfiniteItem(info->Dwarf, Item::ItemSlot::Weapon, info->WeaponIndex, typeInfo.Weapons);
                giveFirstInfiniteItem(info->Dwarf, Item::ItemSlot::Trinket, info->TrinketIndex, typeInfo.Trinkets);
                giveFirstInfiniteItem(info->Dwarf, Item::ItemSlot::Armor, info->ArmorIndex, typeInfo.Armors);
                info->IsEquiped = true;
            }

            void selectDwarf(LoadoutDwarfInfo* info)
            {
                _selectedDwarf = info;

                // Move selected dwarf to the chest
                if (_selectedDwarf)
                {
                    _selectedDwarf->Dwarf->PushAction(CreateMoveAction(_chestPos), false);
                }
                else
                {
                    if (_loadoutPanelIn)
                    {
                        animateLoadoutOut();
                    }
                }

                repositionReadyDwarves();
            }

            void repositionReadyDwarves()
            {
                if (_dwarves.size() == 0)
                {
                    return;
                }

                float totalWidth = 0.0f;
                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    totalWidth += _dwarves[i]->Width;
                }

                Vector2f rightDir = Vector2f::Normalize(_readyPosLeft - _readyPosRight);
                Vector2f curPos = _readyPosRight;

                float readyWidth = Vector2f::Distance(_readyPosRight, _readyPosLeft);
                bool widthConstrained = totalWidth > readyWidth;

                for (uint32_t i = 0; i < _dwarves.size(); i++)
                {
                    bool selected = _selectedDwarf && _dwarves[i]->Dwarf == _selectedDwarf->Dwarf;
                    if (!selected)
                    {
                        _dwarves[i]->Dwarf->PushAction(CreateMoveAction(curPos), false);
                    }

                    if (widthConstrained)
                    {
                        float widthPerc = _dwarves[i]->Width / totalWidth;
                        curPos += (rightDir * (readyWidth * widthPerc));
                    }
                    else
                    {
                        curPos += (rightDir * _dwarves[i]->Width);
                    }
                }
            }

            template <typename DwarfType>
            void insertDefaultDwarfInfo(DwarfConstructor<> constructor, const std::string& portraitMaterialPath)
            {
                std::type_index type = typeid(DwarfType);

                bool dwarfTypeAvailable = true;
                uint32_t minTypeCounter = 0;
                for (auto minType : _levelInfo.MinTypedCharacters)
                {
                    if (minType.first == type && minType.second > 0)
                    {
                        dwarfTypeAvailable = true;
                        break;
                    }
                    else
                    {
                        minTypeCounter += minType.second;
                        if (minTypeCounter >= _levelInfo.MaxTotalCharacters)
                        {
                            dwarfTypeAvailable = false;
                            break;
                        }
                    }
                }

                if (_profile->IsCharacterTypeUnlocked(dwarfTypeToKey(type)) && dwarfTypeAvailable)
                {
                    dwarfTypeInfo typeInfo;
                    typeInfo.MinCharacters = _levelInfo.MinTypedCharacters[type];
                    typeInfo.Constructor = constructor;
                    typeInfo.TypeState.Weapons = _profile->GetWeaponInfo(dwarfTypeToKey(type));
                    typeInfo.TypeState.Armors = _profile->GetArmorInfo(dwarfTypeToKey(type));
                    typeInfo.TypeState.Trinkets = _profile->GetTrinketInfo(dwarfTypeToKey(type));

                    typeInfo.HintColor = HUD::GetDwarfTypeColor(type);

                    typeInfo.PortraitMaterialPath = portraitMaterialPath;
                    typeInfo.Portrait = nullptr;

                    _dwarfTypes[type] = typeInfo;
                }
            }

            void initializeDwarfTypes()
            {
                insertDefaultDwarfInfo<BrewerDwarf>(BindDwarfConstructor<BrewerDwarf>(), BrewerPortraitMaterialPath);
                insertDefaultDwarfInfo<BuilderDwarf>(BindDwarfConstructor<BuilderDwarf>(), BuilderPortraitMaterialPath);
                insertDefaultDwarfInfo<CookDwarf>(BindDwarfConstructor<CookDwarf>(), CookPortraitMaterialPath);
                insertDefaultDwarfInfo<FighterDwarf>(BindDwarfConstructor<FighterDwarf>(), FighterPortraitMaterialPath);
                insertDefaultDwarfInfo<MinerDwarf>(BindDwarfConstructor<MinerDwarf>(), MinerPortraitMaterialPath);
                insertDefaultDwarfInfo<NavigatorDwarf>(BindDwarfConstructor<NavigatorDwarf>(), NavigatorPortraitMaterialPath);
            }

            void initializeTriggerPositions()
            {
                Level::LevelInstance* level = GetLevel();
                Level::LevelLayerInstance *primaryLayer = level->GetPrimaryLayer();

                _spawnPos = primaryLayer->GetTriggerPosition("spawn_area");
                _removePos = primaryLayer->GetTriggerPosition("remove_area");

                const Polygonf& readyArea = primaryLayer->GetTriggerArea("ready_area");
                _readyPosLeft = Vector2f(readyArea.Bounds().Left(), readyArea.Bounds().Middle().Y);
                _readyPosRight = Vector2f(readyArea.Bounds().Right(), readyArea.Bounds().Middle().Y);

                _chestPos = primaryLayer->GetTriggerPosition("chest_area");

                _menuPos = primaryLayer->GetTriggerPosition("menu");
                _goPos = primaryLayer->GetTriggerPosition("go");

                _portraitPos = primaryLayer->GetTriggerPositions("portrait");

                _chalkboardPos = primaryLayer->GetTriggerPosition("chalkboard");
            }

            struct dwarfTypeInfo
            {
                DwarfConstructor<> Constructor;
                DwarfTypeState TypeState;

                uint32_t MinCharacters;

                Color HintColor;

                std::string PortraitMaterialPath;
                Animation::SkeletonInstance* Portrait;
                Lights::PolygonMaterialLight PortraitLight;

                HUD::Panel* Tooltip;

                uint32_t FirstChalkMan;

                std::set<uint32_t> CheckedOutCharacterInfos;
            };
            typedef std::map<std::type_index, dwarfTypeInfo> DwarfTypeMap;
            DwarfTypeMap _dwarfTypes;

            HUD::Panel* _genericTypeTooltip;

            Level::CampaignLevelInfo _levelInfo;

            Vector2f _spawnPos;
            Vector2f _removePos;
            Vector2f _readyPosLeft;
            Vector2f _readyPosRight;
            Vector2f _chestPos;
            Vector2f _menuPos;
            Vector2f _goPos;
            std::vector<Vector2f> _portraitPos;
            Vector2f _chalkboardPos;

            bool _createdADwarf;
            bool _showedCreateDwarfTutorial;
            float _totalLevelTime;

            HUD::LoadoutPanel* _loadoutPanel;
            bool _loadoutPanelIn;

            HUD::GoPanel* _goPanel;
            bool _goPanelIn;

            Input::InputBindCode _hudSelectBind;

            LoadoutDwarfInfo* _selectedDwarf;

            typedef std::vector<LoadoutDwarfInfo*> DwarfVector;
            DwarfVector _dwarves;
            DwarfVector _removalPendingDwarves;

            Animation::SkeletonInstance* _chalkboard;
            uint32_t _firstGenericChalkman;
            std::vector<Animation::SkeletonInstance*> _chalkmen;

            HUD::TutorialDisplay* _tutorialDisplay = nullptr;

            const HUD::Panel* _curTooltip;

            Vector2f _mousePosScreen;
            Rectanglef _curViewport;

            const Graphics::HUDMaterialSet* _hudMatSet;

            const App::Cursor* _cursor;

            const Graphics::Font* _hudFont;

            LevelChangeFunction _levelChangeFunc;
            bool _startedPreload;
            PreloadLevelFunction _preloadLevelFunc;
            TransitionToCampaignMenuFunction _menuFunc;
            ExitGameFunction _exitFunc;
            PauseGameFunction _pauseFunc;
            SaveProfileFunction _saveProfileFunc;
            Level::CampaignLevelConstructor<> _campaignLevel;

            Settings::TheDeepDeepProfile* _profile;
            float _uiScale;
        };
    }

    template <>
    void EnumeratePreloads<Character::LoadoutController>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Controller>(preloads);

        EnumeratePreloads<HUD::LoadoutPanel>(preloads);
        EnumeratePreloads<HUD::GoPanel>(preloads);

        preloads.insert(Character::PortraitSkeletonPath);
        preloads.insert(Character::BrewerPortraitMaterialPath);
        preloads.insert(Character::BuilderPortraitMaterialPath);
        preloads.insert(Character::CookPortraitMaterialPath);
        preloads.insert(Character::FighterPortraitMaterialPath);
        preloads.insert(Character::MinerPortraitMaterialPath);
        preloads.insert(Character::NavigatorPortraitMaterialPath);

        preloads.insert(Character::ChalkboardSkeletonPath);
        preloads.insert(Character::ChalkboardMaterialPath);

        preloads.insert(Character::ChalkManSkeletonPath);
        preloads.insert(Character::ChalkManMaterialPaths.begin(), Character::ChalkManMaterialPaths.end());
        preloads.insert(Character::ChalkManCrossMaterialPaths.begin(), Character::ChalkManCrossMaterialPaths.end());

        preloads.insert(Character::HUDMaterialSetPath);
        preloads.insert(Character::HUDFontPath);

        EnumeratePreloads<Character::BrewerDwarf>(preloads);
        EnumeratePreloads<Character::BuilderDwarf>(preloads);
        EnumeratePreloads<Character::CookDwarf>(preloads);
        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Character::MinerDwarf>(preloads);
        EnumeratePreloads<Character::NavigatorDwarf>(preloads);

        EnumeratePreloads<HUD::TutorialDisplay>(preloads);

        EnumerateTooltipPreloads(preloads);
        preloads.insert("HUD/hud.cursorset");
    }

    namespace Level
    {
        static const std::map<Audio::EnvironmentType, std::pair<Audio::SoundPathVector, float>> LoadoutEnvironementSoundPaths
        {
            {
                Audio::EnvironmentType::WoodRoom,
                {
                    {
                        { "Audio/Ambiance/Ambience_BarLoop_Stereo.ogg", 1.0f },
                    },
                    1.0f
                },
            },
        };

        LoadoutLevel::LoadoutLevel(const LevelParameters& parameters, const CampaignLevelInfo& campaignLevelInfo, LevelChangeFunction levelChangeFunc,
                                   PreloadLevelFunction preloadLevelFunc, ExitGameFunction exitFunc, TransitionToCampaignMenuFunction menuFunc,
                                   PauseGameFunction pauseFunc, SaveProfileFunction saveProfileFunc, CampaignLevelConstructor<> campaignLevel)
            : BasicLevel(parameters)
            , _campaignLevelInfo(campaignLevelInfo)
            , _levelChangeFunc(levelChangeFunc)
            , _preloadLevelFunc(preloadLevelFunc)
            , _exitFunc(exitFunc)
            , _menuFunc(menuFunc)
            , _pauseFunc(pauseFunc)
            , _saveProfileFunc(saveProfileFunc)
            , _campaignLevel(campaignLevel)
        {
            for (const auto& environmentSounds : LoadoutEnvironementSoundPaths)
            {
                AddAmbientSounds(environmentSounds.first, environmentSounds.second.first, environmentSounds.second.second);
            }
        }

        LoadoutLevel::~LoadoutLevel()
        {
        }

        void LoadoutLevel::OnCreate()
        {
            BasicLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::WoodRoom);

            CreateController("controller", Character::BindControllerConstructor<Character::LoadoutController>(_campaignLevelInfo,
                                                                                                              _levelChangeFunc,
                                                                                                              _preloadLevelFunc,
                                                                                                              _exitFunc,
                                                                                                              _menuFunc,
                                                                                                              _pauseFunc,
                                                                                                              _saveProfileFunc,
                                                                                                              _campaignLevel));

            SpawnCharactersAtTriggers("brazier_wall_spawn", Character::BindCharacterConstructor<Character::WallBrazier>(BaseFireColor, false, true, false), "wall_brazier", NULL);
            SpawnCharactersAtTriggers("brazier_wall_sound_spawn", Character::BindCharacterConstructor<Character::WallBrazier>(BaseFireColor, false, true, true), "wall_brazier", NULL);
            SpawnCharactersAtTriggers("brazier_floor_spawn", Character::BindCharacterConstructor<Character::FloorBrazier>(BaseFireColor, false, true, false), "floor_brazier", NULL);
            SpawnCharactersAtTriggers("brazier_hang_spawn", Character::BindCharacterConstructor<Character::HangingBrazier>(BaseFireColor, false, true, false), "hang_brazier", NULL);
            SpawnCharactersAtTriggers("candle_large_spawn", Character::BindCharacterConstructor<Character::SmallCandle>(BaseFireColor, false, true, false), "small_candle", NULL);
            SpawnCharactersAtTriggers("candle_small_spawn", Character::BindCharacterConstructor<Character::LargeCandle>(BaseFireColor, false, true, false), "large_candle", NULL);

            CameraController& cameraController = GetCameraController();
            const Polygonf& cameraArea = cameraController.GetBounds();
            const Rectanglef& cameraBounds = cameraArea.Bounds();

            cameraController.Pan(cameraBounds.Middle());
            cameraController.Rotate(0.0f);

            SetTargetCameraViewSize(cameraBounds.H, 16.0f / 9.0f);
        }
    }

    template <>
    void EnumeratePreloads<Level::LoadoutLevel>(PreloadSet& preloads)\
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
        EnumeratePreloads<Character::LoadoutController>(preloads);
        EnumeratePreloads<Character::WallBrazier>(preloads);
        EnumeratePreloads<Character::FloorBrazier>(preloads);
        EnumeratePreloads<Character::HangingBrazier>(preloads);
        EnumeratePreloads<Character::SmallCandle>(preloads);
        EnumeratePreloads<Character::LargeCandle>(preloads);

        for (const auto& environmentSounds : Level::LoadoutEnvironementSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, environmentSounds.second.first);
        }
    }
}
