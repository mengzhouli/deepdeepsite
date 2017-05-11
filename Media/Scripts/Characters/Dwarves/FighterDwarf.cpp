#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Item/Weapon.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string FighterMatsetPath = "Skeletons/Characters/Dwarf2/brawler.polymatset";
        static const std::string FighterIconMatsetPath = "HUD/hud.hudmatset";

        FighterDwarf::FighterDwarf(const CharacterParameters& parameters)
            : FighterDwarf(parameters, CreateDefaultDwarfInfo(parameters))
        {
        }

        FighterDwarf::FighterDwarf(const CharacterParameters& parameters, const DwarfInfo& info)
            : Dwarf(parameters, info, FighterMatsetPath)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 2);
            SetIcon(FighterIconMatsetPath, "class_fighter");
            setupRunAnimations();
        }

        void FighterDwarf::OnEquipItem(Item::Item* item)
        {
            Dwarf::OnEquipItem(item);
            setupRunAnimations();
        }

        void FighterDwarf::OnUnequipItem(Item::Item* item)
        {
            Dwarf::OnUnequipItem(item);
            setupRunAnimations();
        }

        void FighterDwarf::setupRunAnimations()
        {
            bool hasShield = false;
            for (auto weapon : GetWeapons())
            {
                if (weapon->GetWeaponType() == Item::WeaponType::WeaponType_Melee_Shield)
                {
                    hasShield = true;
                    break;
                }
            }

            ClearTerrainAnimations(AnimationType_Move, Pathfinding::EdgeType_Walk);
            if (hasShield)
            {
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "RUN_4", 1.0f);
            }
            else
            {
                AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, ForwardRunAnimation, 1.0f);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::FighterDwarf>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Dwarf>(preloads);
        preloads.insert(Character::FighterMatsetPath);
        preloads.insert(Character::FighterIconMatsetPath);
    }
}
