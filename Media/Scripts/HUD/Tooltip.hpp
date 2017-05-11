#pragma once

#include "HUD/Panel.hpp"
#include "HUD/Icon.hpp"
#include "Content/ContentManager.hpp"
#include "Character/Damage.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Localization/StringTable.hpp"
#include "Item/ItemTypes.hpp"
#include "Ability/AbilityTypes.hpp"
#include "Level/LevelInstance.hpp"
#include "Levels/CampaignLevel.hpp"
#include "Application/Cursor.hpp"
#include "Settings/TheDeepDeepProfile.hpp"
#include "Tutorials.hpp"

#include "HUD/PanelBorder.hpp"

#include <string>
#include <typeindex>

namespace Dwarf
{
    namespace HUD
    {
        const Color& GetDwarfTypeColor(const std::type_index& type);

        BorderPanelDrawable* CreateThinBorder(Content::ContentManager* contentManager);
        BorderPanelDrawable* CreateMediumBorder(Content::ContentManager* contentManager);
        BorderPanelDrawable* CreateThickBorder(Content::ContentManager* contentManager);

        BorderPanelDrawable* CreateTutorialBorder(Content::ContentManager* contentManager, TutorialArrowCorner corner);

        PanelDrawable* CreateScrollUpButton(Content::ContentManager* contentManager);
        PanelDrawable* CreateScrollDownButton(Content::ContentManager* contentManager);
        PanelDrawable* CreateScrollDragButton(Content::ContentManager* contentManager);

        BorderPanelDrawable* CreateButtonBorder(Content::ContentManager* contentManager);
        PanelDrawable* CreateButtonHighlight(Content::ContentManager* contentManager);
        BorderPanelDrawable* CreateButtonCornerBorder(Content::ContentManager* contentManager);

        BorderPanelDrawable* CreateCheckboxBorder(Content::ContentManager* contentManager);
        PanelDrawable* CreateCheckboxHighlight(Content::ContentManager* contentManager);
        PanelDrawable* CreateCheckboxCheck(Content::ContentManager* contentManager);

        BorderPanelDrawable* CreateComboboxBorder(Content::ContentManager* contentManager);
        BorderPanelDrawable* CreateComboboxDropDownBorder(Content::ContentManager* contentManager);
        PanelDrawable* CreateComboboxHighlight(Content::ContentManager* contentManager);
        PanelDrawable* CreateComboboxDownButton(Content::ContentManager* contentManager);

        PanelDrawable* CreateItemIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                      Animation::SkeletonInstance* itemSkeleton);

        PanelDrawable* CreateCharacterIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                           Animation::SkeletonInstance* itemSkeleton);

        PanelDrawable* CreateAbilityIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                         HUD::Icon*& outIcon);
        
        PanelDrawable* CreateBuffIcon(Content::ContentManager* contentManager, const std::string& matsetPath, const std::string& materialName,
                                      HUD::Icon*& outIcon);
        
        const App::Cursor* CreateCursor(Content::ContentManager* contentManager, const std::string& cursorsetPath, const std::string& cursorName);
        const App::Cursor* CreateCursor(Content::ContentManager* contentManager, const std::string& cursorPath);

        Panel* CreateWeaponTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                        Item::Weapon* item, const std::string& description, const std::string& flavor);

        Panel* CreateArmorTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                       Item::Armor* item, const std::string& description, const std::string& flavor);

        Panel* CreateTrinketTooltipPanel(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                         Item::Trinket* item, const std::string& description, const std::string& flavor);

        Panel* CreateAbilityTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                    const std::string& name, const PanelDrawable* icon, Ability::AbilityType type,
                                    const Item::Resources& cost, float cooldown, const std::string& description, const std::string& flavor);

        Panel* CreateBuffTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                 const std::string& name, const PanelDrawable* icon,
                                 const std::string& description, const std::string& flavor);

        Panel* CreateBasicActionTooltip(Content::ContentManager* contentManager, const std::string& name, const PanelDrawable* icon,
                                        const std::string& description);

        Panel* CreateSimpleTextTooltip(Content::ContentManager* contentManager, const std::string& text);

        Panel* CreateCampaignLevelTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Level::CampaignLevelInfo& info,
                                          const Settings::TheDeepDeepProfile* profile);

        TutorialArrowCorner GetTutorialArrowCorner(TutorialType tutorial);
        Panel* CreateTutorialTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings,
                                     TutorialType tutorial, const Settings::TheDeepDeepProfile* profile);

        Panel* CreateBridgeTooltip(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Item::Resources& remainingCost);

    }

    void EnumerateTooltipPreloads(PreloadSet& preloads);
}
