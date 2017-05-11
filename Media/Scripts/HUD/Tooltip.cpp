#include "HUD/Tooltip.hpp"

#include "HUD/SkeletonPanelDrawable.hpp"

#include "Application/CursorSet.hpp"

#include "Item/Resources.hpp"

#include "DamageTypes.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

#include "Item/Weapon.hpp"
#include "Item/Armor.hpp"
#include "Item/Trinket.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"

#include <string>

namespace Dwarf
{
    namespace HUD
    {
        const std::string TooltipNameFont = "Fonts/hud_font.ttf";
        const std::string IconFont = "Fonts/hud_font.ttf";

        const std::string IconMaterialSet = "HUD/hud.hudmatset";
        const std::string BorderMaterialSet = "HUD/hud.hudmatset";

        const std::string LargeBorderMaterialTopLeft = "tooltip_corner_large_top_left";
        const std::string LargeBorderMaterialTopRight = "tooltip_corner_large_top_right";
        const std::string LargeBorderMaterialBottomLeft = "tooltip_corner_large_bottom_left";
        const std::string LargeBorderMaterialBottomRight = "tooltip_corner_large_bottom_right";
        const std::string LargeBorderMaterialEdgeLeft = "tooltip_corner_large_edge_left";
        const std::string LargeBorderMaterialEdgeRight = "tooltip_corner_large_edge_right";
        const std::string LargeBorderMaterialEdgeTop = "tooltip_corner_large_edge_top";
        const std::string LargeBorderMaterialEdgeBottom = "tooltip_corner_large_edge_bottom";

        const std::string FancyBorderMaterialTopLeft = "tooltip_corner_fancy_top_left";
        const std::string FancyBorderMaterialTopRight = "tooltip_corner_fancy_top_right";
        const std::string FancyBorderMaterialBottomLeft = "tooltip_corner_fancy_bottom_left";
        const std::string FancyBorderMaterialBottomRight = "tooltip_corner_fancy_bottom_right";
        const std::string FancyBorderMaterialEdgeLeft = "tooltip_corner_fancy_edge_left";
        const std::string FancyBorderMaterialEdgeRight = "tooltip_corner_fancy_edge_right";
        const std::string FancyBorderMaterialEdgeTop = "tooltip_corner_fancy_edge_top";
        const std::string FancyBorderMaterialEdgeBottom = "tooltip_corner_fancy_edge_bottom";

        const std::string MediumBorderMaterialTopLeft = "tooltip_corner_medium_top_left";
        const std::string MediumBorderMaterialTopRight = "tooltip_corner_medium_top_right";
        const std::string MediumBorderMaterialBottomLeft = "tooltip_corner_medium_bottom_left";
        const std::string MediumBorderMaterialBottomRight = "tooltip_corner_medium_bottom_right";
        const std::string MediumBorderMaterialEdgeLeft = "tooltip_corner_medium_edge_left";
        const std::string MediumBorderMaterialEdgeRight = "tooltip_corner_medium_edge_right";
        const std::string MediumBorderMaterialEdgeTop = "tooltip_corner_medium_edge_top";
        const std::string MediumBorderMaterialEdgeBottom = "tooltip_corner_medium_edge_bottom";

        const std::string ThinBorderMaterialTopLeft = "tooltip_corner_thin_top_left";
        const std::string ThinBorderMaterialTopRight = "tooltip_corner_thin_top_right";
        const std::string ThinBorderMaterialBottomLeft = "tooltip_corner_thin_bottom_left";
        const std::string ThinBorderMaterialBottomRight = "tooltip_corner_thin_bottom_right";
        const std::string ThinBorderMaterialEdgeLeft = "tooltip_corner_thin_edge_left";
        const std::string ThinBorderMaterialEdgeRight = "tooltip_corner_thin_edge_right";
        const std::string ThinBorderMaterialEdgeTop = "tooltip_corner_thin_edge_top";
        const std::string ThinBorderMaterialEdgeBottom = "tooltip_corner_thin_edge_bottom";

        const std::string DamgeTypeTitleString = "damage_type_title";
        const std::map<Character::DamageType, std::pair<std::string, std::string>> DamageTypeStrings =
        {
            { Character::DamageType_Type_Melee, { "damage_type_melee", "white" } },
            { Character::DamageType_Type_Projectile, { "damage_type_projectile", "white" } },
            { Character::DamageType_Type_Explosion, { "damage_type_explosion", "red" } },
            { Character::DamageType_Element_Physical, { "damage_type_physical", "white" } },
            { Character::DamageType_Element_Fire, { "damage_type_fire", "orange" } },
            { Character::DamageType_Element_Poison, { "damage_type_poison", "forestgreen" } },
        };
        const std::map<Item::WeaponType, std::string> WeaponTypeStrings = 
        {
            { Item::WeaponType_Melee_Hands, "weapon_type_melee_hands" },
            { Item::WeaponType_Melee_1H, "weapon_type_melee_1h" },
            { Item::WeaponType_Melee_2H, "weapon_type_melee_2h" },
            { Item::WeaponType_Melee_Shield, "weapon_type_melee_shield" },
            { Item::WeaponType_Ranged_Bow, "weapon_type_bow" },
            { Item::WeaponType_Ranged_Gun_1H, "weapon_type_gun_1h" },
            { Item::WeaponType_Ranged_Gun_2H, "weapon_type_gun_2h" },
            { Item::WeaponType_Ranged_Thrown, "weapon_type_thrown" },
            { Item::WeaponType_Special, "weapon_type_special" },
        };
        const std::string WeaponDamageTitleString = "weapon_damage_title";
        const std::string WeaponDamageUnitsString = "weapon_damage_units";
        const std::string WeaponRangeTitleString = "weapon_range_title";
        const std::string WeaponRangeMeleeString = "weapon_range_melee";
        const std::string WeaponBlockTitleString = "weapon_block_title";
        const std::string WeaponBlockDescriptionString = "weapon_block_description";
        const std::string WeaponBlockDamageSuffixString = "weapon_block_damage_suffix";
        const std::string WeaponCritChanceTitleString = "weapon_crit_chance_title";
        const std::string WeaponCritDamageTitleString = "weapon_crit_damage_title";

        static const std::string ChalkManSkeletonPath = "Skeletons/UI/Chalkboard/chalk_man.skel";
        static const std::vector<std::string> ChalkManMaterialPaths
        {
            "Skeletons/UI/Chalkboard/chalk_man1.polymatset",
            "Skeletons/UI/Chalkboard/chalk_man2.polymatset",
            "Skeletons/UI/Chalkboard/chalk_man3.polymatset",
        };
        static const std::string ChalkManUncrossAnimation = "uncross";
        const std::string RequiredCharactersString = "level_required_characters";
        const std::string UnlockedItemsString = "level_unlocked_items";
        const std::string UnlockedCharactersString = "level_unlocked_characters";
        const std::string UnlockedAbilitiesString = "level_unlocked_abilities";
        const std::string UnlockedSeperatorString = "level_unlocked_seperator";

        const std::set<Item::WeaponType> MeleeWeaponTypes = 
        {
            Item::WeaponType_Melee_Hands,
            Item::WeaponType_Melee_1H,
            Item::WeaponType_Melee_2H,
            Item::WeaponType_Melee_Shield,
        };

        const std::set<Item::WeaponType> RangedWeaponTypes =
        {
            Item::WeaponType_Ranged_Bow,
            Item::WeaponType_Ranged_Gun_1H,
            Item::WeaponType_Ranged_Gun_2H,
            Item::WeaponType_Ranged_Thrown,
        };

        const std::string MiningRateTitleString = "item_mining_rate_title";
        const std::string MiningRateSuffixString = "item_mining_rate_suffix";
        const std::string MiningRateModifierTitleString = "item_mining_rate_modifier_title";

        const std::string ArmorArmorTitleString = "armor_armor_title";

        const std::string CooldownTitleString = "ability_cooldown_title";
        const std::string CooldownUnitsString = "ability_cooldown_units";
        const std::string AbilityCostTitleString = "ability_cost_title";
        const std::string AbilityCostGoldUnitsString = "ability_cost_gold_units";
        const std::string AbilityCostWoodUnitsString = "ability_cost_wood_units";

        const float DefaultIconSize = 96.0f;
        const float DefaultTooltipWidth = 512.0f;
        const float DefaultTutorialWidth = 256.0f;

        const Color& GetDwarfTypeColor(const std::type_index& type)
        {
            static const std::map<std::type_index, Color> colors
            {
                { std::type_index(typeid(Character::BrewerDwarf)), Color::FromBytes(172, 58, 51, 255) },
                { std::type_index(typeid(Character::BuilderDwarf)), Color::FromBytes(62, 172, 105, 255) },
                { std::type_index(typeid(Character::CookDwarf)), Color::FromBytes(214, 204, 190, 255) },
                { std::type_index(typeid(Character::FighterDwarf)), Color::FromBytes(210, 148, 61, 255) },
                { std::type_index(typeid(Character::MinerDwarf)), Color::FromBytes(160, 98, 58, 255) },
                { std::type_index(typeid(Character::NavigatorDwarf)), Color::FromBytes(65, 105, 146, 255) },
            };

            auto iter = colors.find(type);
            assert(iter != colors.end());
            return iter->second;
        }

        BorderPanelDrawable* CreateFancyBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new TilingBorderPanelDrawable(borderMatset, FancyBorderMaterialTopLeft, FancyBorderMaterialTopRight, FancyBorderMaterialBottomLeft,
                                                FancyBorderMaterialBottomRight, FancyBorderMaterialEdgeLeft, FancyBorderMaterialEdgeRight,
                                                FancyBorderMaterialEdgeTop, FancyBorderMaterialEdgeBottom);
        }

        BorderPanelDrawable* CreateThinBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new TilingBorderPanelDrawable(borderMatset, ThinBorderMaterialTopLeft, ThinBorderMaterialTopRight, ThinBorderMaterialBottomLeft,
                                                ThinBorderMaterialBottomRight, ThinBorderMaterialEdgeLeft, ThinBorderMaterialEdgeRight,
                                                ThinBorderMaterialEdgeTop, ThinBorderMaterialEdgeBottom);
        }

        BorderPanelDrawable* CreateMediumBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new TilingBorderPanelDrawable(borderMatset, MediumBorderMaterialTopLeft, MediumBorderMaterialTopRight, MediumBorderMaterialBottomLeft,
                                                MediumBorderMaterialBottomRight, MediumBorderMaterialEdgeLeft, MediumBorderMaterialEdgeRight,
                                                MediumBorderMaterialEdgeTop, MediumBorderMaterialEdgeBottom);
        }

        BorderPanelDrawable* CreateThickBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new TilingBorderPanelDrawable(borderMatset, LargeBorderMaterialTopLeft, LargeBorderMaterialTopRight, LargeBorderMaterialBottomLeft,
                                                LargeBorderMaterialBottomRight, LargeBorderMaterialEdgeLeft, LargeBorderMaterialEdgeRight,
                                                LargeBorderMaterialEdgeTop, LargeBorderMaterialEdgeBottom);
        }

        BorderPanelDrawable* CreateTutorialBorder(Content::ContentManager* contentManager, TutorialArrowCorner corner)
        {
            const Vector2f padding(30, 20);
            const float scale = 0.5f;

            std::string topMaterial = "tutorial_top_no_arrow";
            std::string botMaterial = "tutorial_bottom_no_arrow";

            switch (corner)
            {
            case TutorialArrowCorner::None:
                break;

            case TutorialArrowCorner::TopLeft:
                topMaterial = "tutorial_top_left_arrow";
                break;

            case TutorialArrowCorner::TopRight:
                topMaterial = "tutorial_top_right_arrow";
                break;

            case TutorialArrowCorner::BottomLeft:
                botMaterial = "tutorial_bottom_left_arrow";
                break;

            case TutorialArrowCorner::BottomRight:
                botMaterial = "tutorial_bottom_right_arrow";
                break;

            default: assert(false); break;
            }

            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new VerticalStretchingBorderPanelDrawable(borderMatset, topMaterial, "tutorial_middle", botMaterial, padding, scale);
        }

        PanelDrawable* CreateScrollUpButton(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("scroll_up_button"));
        }

        PanelDrawable* CreateScrollDownButton(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("scroll_down_button"));
        }

        PanelDrawable* CreateScrollDragButton(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("scroll_drag_button"));
        }

        BorderPanelDrawable* CreateButtonBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new StretchingBorderPanelDrawable(borderMatset, "button_frame", "button_frame_inner");
        }

        PanelDrawable* CreateButtonHighlight(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("button_highlight"));
        }

        BorderPanelDrawable* CreateButtonCornerBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new StretchingBorderPanelDrawable(borderMatset, "button_corner_frame", "button_corner_frame_inner");
        }

        BorderPanelDrawable* CreateCheckboxBorder(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new StretchingBorderPanelDrawable(borderMatset, "button_frame", "button_frame_inner");
        }

        PanelDrawable* CreateCheckboxHighlight(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("button_highlight"));
        }

        PanelDrawable* CreateCheckboxCheck(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("checkbox_check"));
        }

        BorderPanelDrawable* CreateComboboxBorder(Content::ContentManager* contentManager)
        {
            return CreateThinBorder(contentManager);
        }

        BorderPanelDrawable* CreateComboboxDropDownBorder(Content::ContentManager* contentManager)
        {
            return CreateThinBorder(contentManager);
        }

        PanelDrawable* CreateComboboxHighlight(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> borderMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(borderMatset->GetMaterial("button_highlight"));
        }

        PanelDrawable* CreateComboboxDownButton(Content::ContentManager* contentManager)
        {
            ResourcePointer<const Graphics::HUDMaterialSet> matset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(BorderMaterialSet));
            return new HUDMaterialPanelDrawable(matset->GetMaterial("scroll_down_button"));
        }

        PanelDrawable* CreateItemIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                      Animation::SkeletonInstance* itemSkeleton)
        {
            if (matsetPath.length() > 0 && materialName.length() > 0)
            {
                ResourcePointer<const Graphics::HUDMaterialSet> matset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(matsetPath));
                return new HUDMaterialPanelDrawable(matset->GetMaterial(materialName));
            }
            else if (itemSkeleton)
            {
                // HACK: At least show something in the tooltip if there wasn't one given
                for (uint32_t i = 0; i < itemSkeleton->GetJointCount(); i++)
                {
                    const Graphics::PolygonMaterial* jointMat = itemSkeleton->GetMaterial(i);
                    if (jointMat)
                    {
                        ResourcePointer<Graphics::HUDMaterial> hudMaterial = MakeResource<Graphics::HUDMaterial>(jointMat->GetTexture(), jointMat->GetTexCoords().Bounds(), false);
                        return new HUDMaterialPanelDrawable(hudMaterial);
                    }
                }

                return new NULLPanelDrawable();
            }
            else
            {
                return new NULLPanelDrawable();
            }
        }

        PanelDrawable* CreateCharacterIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                           Animation::SkeletonInstance* itemSkeleton)
        {
            if (matsetPath.length() > 0 && materialName.length() > 0)
            {
                ResourcePointer<const Graphics::HUDMaterialSet> matset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(matsetPath));
                return new HUDMaterialPanelDrawable(matset->GetMaterial(materialName));
            }
            else
            {
                return new NULLPanelDrawable();
            }
        }

        PanelDrawable* CreateAbilityIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                         HUD::Icon*& outIcon)
        {
            ResourcePointer<const Graphics::Font> font = EmplaceResource(contentManager->Load<Graphics::Font>(IconFont));

            outIcon = new HUD::Icon();
            if (matsetPath.length() > 0 && materialName.length() > 0)
            {
                ResourcePointer<const Graphics::HUDMaterialSet> matset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(matsetPath));
                outIcon->SetMaterial(matset->GetMaterial(materialName));
            }

            return new IconPanelDrawable(font, outIcon);
        }
        
        PanelDrawable* CreateBuffIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                      HUD::Icon*& outIcon)
        {
            ResourcePointer<const Graphics::Font> font = EmplaceResource(contentManager->Load<Graphics::Font>(IconFont));

            outIcon = new HUD::Icon();
            if (matsetPath.length() > 0 && materialName.length() > 0)
            {
                ResourcePointer<const Graphics::HUDMaterialSet> matset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(matsetPath));
                outIcon->SetMaterial(matset->GetMaterial(materialName));
            }

            return new IconPanelDrawable(font, outIcon);
        }

        const App::Cursor* CreateCursor(Content::ContentManager* contentManager, const std::string& cursorsetPath, const std::string& cursorName)
        {
            if (cursorsetPath.length() > 0 && cursorName.length() > 0)
            {
                ResourcePointer<const App::CursorSet> cursorSet = EmplaceResource(contentManager->Load<App::CursorSet>(cursorsetPath));
                ResourcePointer<const App::Cursor> cursor = cursorSet->GetCursor(cursorName);
                return cursor.Release();
            }
            else
            {
                return nullptr;
            }
        }

        const App::Cursor* CreateCursor(Content::ContentManager* contentManager, const std::string& cursorPath)
        {
            return contentManager->Load<App::Cursor>(cursorPath);
        }

        static Panel* CreateBasicTooltipPanel(Content::ContentManager* contentManager, const std::string& name, const PanelDrawable* icon,
                                              const std::string& mainDescriptionText, const std::string& extraDescriptionText)
        {
            ResourcePointer<const Graphics::Font> tooltipFont = EmplaceResource(contentManager->Load<Graphics::Font>(TooltipNameFont));

            ResourcePointer<BorderPanelDrawable> largeBorder = EmplaceResource(CreateThickBorder(contentManager));
            ResourcePointer<BorderPanelDrawable> mediumBorder = EmplaceResource(CreateMediumBorder(contentManager));
            ResourcePointer<BorderPanelDrawable> thinBorder = EmplaceResource(CreateThinBorder(contentManager));

            ResourcePointer<Panel> namePanel = MakeResource<Panel>();
            namePanel->AddAlignedText(Alignment_Center, tooltipFont, name);
            namePanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, mediumBorder);
            namePanel->SetVariablePadding(mediumBorder->GetPaddingFunction());

            ResourcePointer<Panel> iconPanel = MakeResource<Panel>();
            iconPanel->AddAlignedPanelDrawable(Alignment_Fill, icon);
            iconPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, thinBorder);
            iconPanel->SetVariablePadding(thinBorder->GetPaddingFunction());
            iconPanel->SetConstantPadding(Vector2f(3.0f, 3.0f));
            iconPanel->SetWidth(DefaultIconSize);
            iconPanel->SetHeight(DefaultIconSize);

            ResourcePointer<Panel> iconOutterPanel = MakeResource<Panel>();
            iconOutterPanel->AddAlignedPanel(Alignment_Top, iconPanel);

            ResourcePointer<Panel> descriptionPanel = MakeResource<Panel>();
            descriptionPanel->SetConstantPadding(Vector2f(3.0f, 0.0f));
            descriptionPanel->AddAlignedText(Alignment_Fill, tooltipFont, mainDescriptionText);

            ResourcePointer<Panel> iconDescriptionPanel = MakeResource<Panel>();
            iconDescriptionPanel->AddAlignedPanel(Alignment_Left, iconOutterPanel);
            iconDescriptionPanel->AddAlignedPanel(Alignment_Fill, descriptionPanel);

            ResourcePointer<Panel> centerPanel = MakeResource<Panel>();
            centerPanel->AddAlignedPanel(Alignment_Top, iconDescriptionPanel);

            if (extraDescriptionText.length() > 0)
            {
                centerPanel->AddAlignedText(Alignment_Fill, tooltipFont, extraDescriptionText);
            }

            centerPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, mediumBorder);
            centerPanel->SetVariablePadding(mediumBorder->GetPaddingFunction());
            centerPanel->SetConstantPadding(5.0f);

            ResourcePointer<Panel> tooltipPanel = MakeResource<Panel>();
            tooltipPanel->SetConstantPadding(5.0f);
            tooltipPanel->AddAlignedPanel(Alignment_Top, namePanel);
            tooltipPanel->AddAlignedPanel(Alignment_Fill, centerPanel);

            ResourcePointer<Panel> root = MakeResource<Panel>();
            root->AddAlignedPanel(Alignment_Fill, tooltipPanel);
            root->AddAlignedPanelDrawable(Alignment_BackgroundFill, largeBorder);
            root->SetVariablePadding(largeBorder->GetPaddingFunction());
            root->SetBackgroundColor(Color::Black);
            root->SetWidth(DefaultTooltipWidth);

            return root.Release();
        }

        void AppendItemTraitsDescriptions(const Localization::StringTable* strings, const Item::Item* item, std::string& descriptionText)
        {
            if (IsA<Item::Miner>(item))
            {
                descriptionText += Format("\n<size=16><color=beige>%s </color><color=white>%.1f</color><color=gray> %s</color></size>",
                                          strings->GetString(MiningRateTitleString).c_str(), AsA<Item::Miner>(item)->GetMiningRate(),
                                          strings->GetString(MiningRateSuffixString).c_str());
            }

            if (IsA<Item::MineRateModifier>(item))
            {
                float multiplier = AsA<Item::MineRateModifier>(item)->GetMineRateMultiplier();
                float percent = (1.0f - multiplier) * 100.0f;

                descriptionText += Format("\n<size=16><color=beige>%s </color><color=white>%0.0f%%</color></size>",
                                          strings->GetString(MiningRateTitleString).c_str(), percent);
            }
        }

        Panel* CreateWeaponTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                        Item::Weapon* item, const std::string& description, const std::string& flavor)
        {
            if (item->GetName().empty())
            {
                return nullptr;
            }

            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", item->GetName().c_str());

            std::string weaponDescriptionText = "";

            Item::WeaponType type = item->GetWeaponType();
            assert(WeaponTypeStrings.find(type) != WeaponTypeStrings.end());
            const std::string& weaponTypeString = WeaponTypeStrings.find(type)->second;
            weaponDescriptionText += Format("<size=16><color=white>%s</color></size>", strings->GetString(weaponTypeString).c_str());

            const Character::Damage& damage = item->GetDPS();
            if (damage.Amount > 0)
            {
                // DPS and damage type text
                weaponDescriptionText += Format("\n<size=16><color=beige>%s </color><color=white>%.1f</color><color=gray> %s</color></size>",
                                                strings->GetString(WeaponDamageTitleString).c_str(), damage.Amount, strings->GetString(WeaponDamageUnitsString).c_str());

                weaponDescriptionText += Format("\n<size=16><color=beige>%s </color>", strings->GetString(DamgeTypeTitleString).c_str());
                uint32_t foundDamageTypes = 0;
                for (auto i = DamageTypeStrings.begin(); i != DamageTypeStrings.end(); i++)
                {
                    Character::DamageType damageType = i->first;
                    if ((damage.Type & damageType) != 0)
                    {
                        if (foundDamageTypes != 0)
                        {
                            weaponDescriptionText += "<color=gray>, </color>";
                        }

                        const std::string& damageTypeString = strings->GetString(i->second.first);
                        const std::string& damageTypeColor = i->second.second;

                        weaponDescriptionText += Format("<color=%s>%s</color>", damageTypeColor.c_str(), damageTypeString.c_str());

                        foundDamageTypes++;
                    }
                }
                weaponDescriptionText += "</size>";

                // Damage range text
                weaponDescriptionText += Format("\n<size=16><color=beige>%s </color>", strings->GetString(WeaponRangeTitleString).c_str());
                if (MeleeWeaponTypes.find(type) != MeleeWeaponTypes.end())
                {
                    weaponDescriptionText += Format("<color=white>%s</color>", strings->GetString(WeaponRangeMeleeString).c_str());
                }
                else if (RangedWeaponTypes.find(type) != RangedWeaponTypes.end())
                {
                    weaponDescriptionText += Format("<color=white>%g</color>", item->GetRange());
                }
                weaponDescriptionText += "</size>";

                // Crit chance
                if (damage.CriticalChance > 0.0f)
                {
                    weaponDescriptionText += Format("\n<size=16><color=beige>%s +%g%%</color></size>", strings->GetString(WeaponCritChanceTitleString).c_str(), damage.CriticalChance * 100.0f);
                }

                // Crit damage
                if (damage.CriticalDamage > 0.0f)
                {
                    weaponDescriptionText += Format("\n<size=16><color=beige>%s +%g%%</color></size>", strings->GetString(WeaponCritDamageTitleString).c_str(), damage.CriticalDamage * 100.0f);
                }
            }

            float armorValue = item->GetArmorValue();
            if (armorValue > 0.0f)
            {
                weaponDescriptionText += Format("\n<size=16><color=beige>%s </color><color=white>%.1f</color></size>",
                                                strings->GetString(ArmorArmorTitleString).c_str(), armorValue);
            }

            const Item::BlockMap& blocks = item->GetBlockChances();
            for (const auto& block : blocks)
            {
                const std::string& damageTypeString = strings->GetString(DamageTypeStrings.at(block.first).first);
                const std::string& damageTypeColor = DamageTypeStrings.at(block.first).second;

                weaponDescriptionText += Format("\n<size=16><color=beige>%s </color><color=white>%0.0f%% </color><color=gray>%s </color><color=white>%0.1f </color>"
                                                "<color=%s>%s </color><color=gray>%s.</color></size>",
                                                "Block:", block.second.Chance * 100.0f, "chance to block", block.second.Ammount,
                                                damageTypeColor.c_str(), damageTypeString.c_str(), "damage");
            }

            AppendItemTraitsDescriptions(strings, item, weaponDescriptionText);

            if (description.length() > 0)
            {
                weaponDescriptionText += Format("\n\n<size=16><color=white>%s</color></size>", description.c_str());
            }

            std::string flavorText = flavor.length() > 0 ? Format("<size=14><color=beige>%s</color></size>", flavor.c_str()) : "";

            return CreateBasicTooltipPanel(contentManager, nameText, item->GetIcon(), weaponDescriptionText, flavorText);
        }

        Panel* CreateArmorTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                       Item::Armor* item, const std::string& description, const std::string& flavor)
        {
            if (item->GetName().empty())
            {
                return nullptr;
            }

            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", item->GetName().c_str());

            std::string descriptionText = Format("<size=16><color=beige>%s </color><color=white>%.1f</color></size>",
                                                 strings->GetString(ArmorArmorTitleString).c_str(), item->GetArmorValue());

            AppendItemTraitsDescriptions(strings, item, descriptionText);

            if (description.length() > 0)
            {
                descriptionText += Format("\n\n<size=16><color=white>%s</color></size>", description.c_str());
            }

            std::string flavorText = "";
            if (flavor.length() > 0)
            {
                flavorText = Format("<size=14><color=beige><i>%s</i></color></size>", flavor.c_str());
            }

            return CreateBasicTooltipPanel(contentManager, nameText, item->GetIcon(), descriptionText, flavorText);
        }

        Panel* CreateTrinketTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                         Item::Trinket* item, const std::string& description, const std::string& flavor)
        {
            if (item->GetName().empty())
            {
                return nullptr;
            }

            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", item->GetName().c_str());

            std::string descriptionText;
            AppendItemTraitsDescriptions(strings, item, descriptionText);

            if (description.length() > 0)
            {
                descriptionText += Format("\n\n<size=16><color=white>%s</color></size>", description.c_str());
            }

            std::string flavorText = flavor.length() > 0 ? Format("<size=14><color=beige><i>%s</i></color></size>", flavor.c_str()) : "";

            return CreateBasicTooltipPanel(contentManager, nameText, item->GetIcon(), descriptionText, flavorText);
        }

        Panel* CreateAbilityTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                    const std::string& name, const PanelDrawable* icon, Ability::AbilityType type,
                                    const Item::Resources& cost, float cooldown, const std::string& description, const std::string& flavor)
        {
            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", name.c_str());

            std::string cooldownText = "";
            if (cooldown > 0.0f)
            {
                cooldownText += Format("<size=16><color=beige>%s </color><color=white>%.0f</color><color=gray> %s</color></size>",
                                       strings->GetString(CooldownTitleString).c_str(), cooldown, strings->GetString(CooldownUnitsString).c_str());
            }

            std::string costText = "";
            if (cost.Gold > 0)
            {
                costText += Format("<size=16><color=beige>%s </color><color=white>%d</color><color=gray> %s</color></size>",
                                   strings->GetString(AbilityCostTitleString).c_str(), cost.Gold, strings->GetString(AbilityCostGoldUnitsString).c_str());
            }

            std::string abilityUsageText = description.length() > 0 ? Format("<size=16><color=white>%s</color></size>", description.c_str()) : "";

            std::string descriptionText = cooldownText;
            if (costText.length() > 0)
            {
                if (descriptionText.length() > 0)
                {
                    descriptionText += "\n";
                }
                descriptionText += costText;
            }
            if (abilityUsageText.length() > 0)
            {
                if (descriptionText.length() > 0)
                {
                    descriptionText += "\n\n";
                }
                descriptionText += abilityUsageText;
            }

            std::string flavorText = "";
            if (flavor.length() > 0)
            {
                flavorText = Format("<size=14><color=beige><i>%s</i></color></size>", flavor.c_str());
            }

            return CreateBasicTooltipPanel(contentManager, nameText, icon, descriptionText, flavorText);
        }
        
        Panel* CreateBuffTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                 const std::string& name, const PanelDrawable* icon,
                                 const std::string& description, const std::string& flavor)
        {
            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", name.c_str());

            std::string descriptionText = "";
            
            std::string flavorText = "";
            if (flavor.length() > 0)
            {
                flavorText = Format("<size=14><color=beige><i>%s</i></color></size>", flavor.c_str());
            }
            
            return CreateBasicTooltipPanel(contentManager, nameText, icon, descriptionText, flavorText);
        }

        Panel* CreateBasicActionTooltip(Content::ContentManager* contentManager, const std::string& name, const PanelDrawable* icon,
                                        const std::string& description)
        {
            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", name.c_str());
            std::string descriptionText = Format("<size=16><color=gray>%s</color></size>", description.c_str());
            return CreateBasicTooltipPanel(contentManager, nameText, icon, descriptionText, "");
        }

        Panel* CreateSimpleTextTooltip(Content::ContentManager* contentManager, const std::string& text)
        {
            ResourcePointer<const Graphics::Font> font = EmplaceResource(contentManager->Load<Graphics::Font>(IconFont));
            std::string formattedText = Format("<size=16><color=white>%s</color></size>", text.c_str());

            ResourcePointer<Panel> namePanel = MakeResource<Panel>();
            namePanel->AddAlignedText(HUD::Alignment_Center, font, formattedText);
            namePanel->SetConstantPadding(3.0f);

            ResourcePointer<BorderPanelDrawable> thinBorder = EmplaceResource(CreateThinBorder(contentManager));

            ResourcePointer<Panel> tooltip = MakeResource<Panel>();
            tooltip->AddAlignedPanelDrawable(HUD::Alignment_BackgroundFill, thinBorder);
            tooltip->AddAlignedPanel(HUD::Alignment_Fill, namePanel);
            tooltip->SetVariablePadding(thinBorder->GetPaddingFunction());
            tooltip->SetBackgroundColor(Color::Black);

            return tooltip.Release();
        }

        HUD::Panel* CreateCampaignLevelTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Level::CampaignLevelInfo& info,
                                               const Settings::TheDeepDeepProfile* profile)
        {
            ResourcePointer<const Graphics::Font> tooltipFont = EmplaceResource(contentManager->Load<Graphics::Font>(TooltipNameFont));

            ResourcePointer<BorderPanelDrawable> largeBorder = EmplaceResource(CreateThickBorder(contentManager));
            ResourcePointer<BorderPanelDrawable> mediumBorder = EmplaceResource(CreateMediumBorder(contentManager));
            ResourcePointer<BorderPanelDrawable> thinBorder = EmplaceResource(CreateThinBorder(contentManager));

            std::string nameText = Format("<size=36><color=orange><b>%s</b></color></size>", strings->GetString(info.NameCode).c_str());

            ResourcePointer<Panel> namePanel = MakeResource<Panel>();
            namePanel->AddAlignedText(Alignment_Center, tooltipFont, nameText);
            namePanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, mediumBorder);
            namePanel->SetVariablePadding(mediumBorder->GetPaddingFunction());

            std::string characterText = Format("<size=16><color=beige>%s: </color></size>\n", strings->GetString(RequiredCharactersString).c_str());

            ResourcePointer<Panel> chalkmanSkeletonPanel = MakeResource<Panel>();
            auto addChalkMan = [&](const Color& color)
            {
                ResourcePointer<const Animation::Skeleton> chalkmanSkeleton = EmplaceResource(contentManager->Load<Animation::Skeleton>(ChalkManSkeletonPath));
                ResourcePointer<const Graphics::PolygonMaterialSet> chalkmanMatset = EmplaceResource(contentManager->Load<Graphics::PolygonMaterialSet>(Random::RandomItem(ChalkManMaterialPaths)));

                ResourcePointer<SkeletonPanelDrawable> chalkmanDrawable = MakeResource<SkeletonPanelDrawable>(chalkmanSkeleton);
                chalkmanDrawable->MergeMaterialSet(chalkmanMatset);
                chalkmanDrawable->SetColor(color);
                chalkmanDrawable->SetPose(ChalkManUncrossAnimation, 1.0f);
                chalkmanDrawable->SetSize(Vector2f(0.0f, 16.0f));

                chalkmanSkeletonPanel->AddAlignedPanelDrawable(Alignment_Left, chalkmanDrawable);
            };

            uint32_t genericDwarves = info.MaxTotalCharacters;
            for (const auto& minTypedCharacter : info.MinTypedCharacters)
            {
                for (uint32_t i = 0; i < minTypedCharacter.second; i++)
                {
                    addChalkMan(GetDwarfTypeColor(minTypedCharacter.first));
                    assert(genericDwarves > 0);
                    genericDwarves--;
                }
            }
            for (uint32_t i = 0; i < genericDwarves; i++)
            {
                addChalkMan(Color::White);
            }

            ResourcePointer<Panel> chalkmanPanel = MakeResource<Panel>();
            chalkmanPanel->AddAlignedText(Alignment_Left, tooltipFont, characterText);
            chalkmanPanel->AddAlignedPanel(Alignment_Fill, chalkmanSkeletonPanel);
            chalkmanPanel->SetHeight(16);

            std::string descriptionText;
            auto writeFractionText = [&](const std::string& prefixString, const Level::UnlockFraction& fraction)
            {
                if (fraction.second > 0)
                {
                    descriptionText +=
                        Format("<size=16><color=beige>%s: </color><color=white>%u</color><color=gray> %s </color><color=white>%u</color></size>\n",
                            strings->GetString(prefixString).c_str(), fraction.first, strings->GetString(UnlockedSeperatorString).c_str(),
                            fraction.second);
                }
            };

            writeFractionText(UnlockedCharactersString, Level::ComputeUnlockedCharacterFraction(profile, info.FindableCharacters));
            writeFractionText(UnlockedItemsString, Level::ComputeUnlockedItemFraction(profile, info.FindableItems));
            writeFractionText(UnlockedAbilitiesString, Level::ComputeUnlockedAbilityFraction(profile, info.FindableAbilities));

            ResourcePointer<Panel> descriptionPanel = MakeResource<Panel>();
            descriptionPanel->SetConstantPadding(Vector2f(3.0f, 0.0f));
            descriptionPanel->AddAlignedPanel(Alignment_Top, chalkmanPanel);
            descriptionPanel->AddAlignedText(Alignment_Fill, tooltipFont, descriptionText);
            descriptionPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, mediumBorder);
            descriptionPanel->SetVariablePadding(mediumBorder->GetPaddingFunction());

            ResourcePointer<Panel> tooltipPanel = MakeResource<Panel>();
            tooltipPanel->SetConstantPadding(5.0f);
            tooltipPanel->AddAlignedPanel(Alignment_Top, namePanel);
            tooltipPanel->AddAlignedPanel(Alignment_Fill, descriptionPanel);

            ResourcePointer<Panel> root = MakeResource<Panel>();
            root->AddAlignedPanel(Alignment_Fill, tooltipPanel);
            root->AddAlignedPanelDrawable(Alignment_BackgroundFill, largeBorder);
            root->SetVariablePadding(largeBorder->GetPaddingFunction());
            root->SetBackgroundColor(Color::Black);
            root->SetWidth(DefaultTooltipWidth);

            return root.Release();
        }

        TutorialArrowCorner GetTutorialArrowCorner(TutorialType tutorial)
        {
            switch (tutorial)
            {
            case TutorialType::LoadoutClickPortrait:
                return TutorialArrowCorner::TopRight;

            case TutorialType::MovingCharacters:
                return TutorialArrowCorner::None;

            case TutorialType::UseAbilities:
                return TutorialArrowCorner::BottomRight;

            case TutorialType::CharacterSelection:
                return TutorialArrowCorner::None;

            case TutorialType::PrimarySelection:
                return TutorialArrowCorner::BottomRight;

            default:
                return TutorialArrowCorner::None;
            }
        }

        Panel* CreateTutorialTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                    TutorialType tutorial, const Settings::TheDeepDeepProfile* profile)
        {
            std::string tutorialText = "";

            switch (tutorial)
            {
            case TutorialType::LoadoutClickPortrait:
                tutorialText = strings->GetString("tutorial_loadout_click_portrait");
                break;

            case TutorialType::MovingCharacters:
                tutorialText = strings->GetString("tutorial_move_characters");
                break;

            case TutorialType::UseAbilities:
                tutorialText = strings->GetString("tutorial_use_abilities");
                break;

            case TutorialType::CharacterSelection:
                tutorialText = strings->GetString("tutorial_character_selection");
                break;

            case TutorialType::PrimarySelection:
                tutorialText = strings->GetString("tutorial_primary_selection");
                break;

            default:
                break;
            }

            ResourcePointer<const Graphics::Font> font = EmplaceResource(contentManager->Load<Graphics::Font>(IconFont));
            std::string description = Format("<size=16><color=black>%s</color></size>", tutorialText.c_str());

            ResourcePointer<BorderPanelDrawable> tutorialBorder = EmplaceResource(CreateTutorialBorder(contentManager, GetTutorialArrowCorner(tutorial)));

            ResourcePointer<Panel> backgroundPanel = MakeResource<Panel>();
            backgroundPanel->AddAlignedPanelDrawable(Alignment_Fill, tutorialBorder);

            ResourcePointer<Panel> textPanel = MakeResource<Panel>();
            textPanel->AddAlignedText(Alignment_Fill, font, description);

            ResourcePointer<Panel> root = MakeResource<Panel>();
            root->AddAlignedPanelDrawable(Alignment_BackgroundFill, tutorialBorder);
            root->AddAlignedPanel(Alignment_Fill, textPanel);
            root->SetVariablePadding(tutorialBorder->GetPaddingFunction());
            root->SetWidth(252);
            root->SetMinimumHeight(80);

            return root.Release();
        }

        Panel* CreateBridgeTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                   const Item::Resources& remainingCost)
        {
            const std::string& tooltipText = strings->GetString("character_bridge_name");

            ResourcePointer<const Graphics::HUDMaterialSet> iconMatset = EmplaceResource(contentManager->Load<Graphics::HUDMaterialSet>(IconMaterialSet));
            ResourcePointer<HUDMaterialPanelDrawable> iconDrawable = MakeResource<HUDMaterialPanelDrawable>(iconMatset->GetMaterial("resource_gold"), Vector2f(16));

            ResourcePointer<const Graphics::Font> font = EmplaceResource(contentManager->Load<Graphics::Font>(IconFont));
            std::string textLeft = Format("<size=16><color=white>%s (</color></size>", tooltipText.c_str());
            std::string textRight = Format("<size=16><color=white>%i)</color></size>", remainingCost.Gold);

            ResourcePointer<Panel> namePanel = MakeResource<Panel>();
            namePanel->AddAlignedText(Alignment_Left, font, textLeft);
            namePanel->AddAlignedPanelDrawable(HUD::Alignment_Left, iconDrawable);
            namePanel->AddAlignedText(Alignment_Left, font, textRight);

            ResourcePointer<Panel> nameBorderPanel = MakeResource<Panel>();
            nameBorderPanel->AddAlignedPanel(Alignment_Fill, namePanel);
            nameBorderPanel->SetConstantPadding(3.0f);

            ResourcePointer<BorderPanelDrawable> thinBorder = EmplaceResource(CreateThinBorder(contentManager));

            ResourcePointer<Panel> tooltip = MakeResource<Panel>();
            tooltip->AddAlignedPanelDrawable(HUD::Alignment_BackgroundFill, thinBorder);
            tooltip->AddAlignedPanel(HUD::Alignment_Fill, nameBorderPanel);
            tooltip->SetVariablePadding(thinBorder->GetPaddingFunction());
            tooltip->SetBackgroundColor(Color::Black);

            return tooltip.Release();
        }
    }

    void EnumerateTooltipPreloads(PreloadSet& preloads)
    {
        preloads.insert(HUD::TooltipNameFont);
        preloads.insert(HUD::IconFont);
        preloads.insert(HUD::IconMaterialSet);
        preloads.insert(HUD::BorderMaterialSet);
    }
}
