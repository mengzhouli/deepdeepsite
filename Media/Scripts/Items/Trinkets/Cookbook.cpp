#include "Items/Trinkets/Cookbook.hpp"
#include "Characters/CharacterTraits.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string CookbookSkeletonPath = "Skeletons/Items/Trinkets/Cookbook/cookbook.skel";
        static const std::string CookbookIconMatsetPath = "HUD/items.hudmatset";

        Cookbook::Cookbook(const ItemParameters& parameters, const std::string nameCode, const std::string& matsetPath)
            : BasicTrinket(parameters, nameCode, CookbookSkeletonPath, matsetPath)
        {
        }

        void Cookbook::OnEquip(Character::Character* newOwner)
        {
            Character::Cooker* cooker = AsA<Character::Cooker>(newOwner);
            if (cooker != nullptr)
            {
                SetOwnerSkeletonAttachPoint(cooker->GetCookbookAttachPointName(), "origin", "b");
            }
            else
            {
                SetOwnerSkeletonAttachPoint("", "", "");
            }

            BasicTrinket::OnEquip(newOwner);
        }

        float Cookbook::GetCookRangeModifier() const
        {
            return 1.0f;
        }

        uint32_t Cookbook::GetCookFoodAdditionalTargetCount() const
        {
            return 0;
        }

        bool Cookbook::CookedFoodDoesAOE() const
        {
            return false;
        }

        bool Cookbook::CookedFoodTargetsEnemies() const
        {
            return false;
        }

        bool Cookbook::CookedFoodHealsOverTime() const
        {
            return false;
        }

        static const std::string DeVermisCoctioneCookbookMatsetPath = "Skeletons/Items/Trinkets/Cookbook/cookbook_2.polymatset";

        DeVermisCoctioneCookbook::DeVermisCoctioneCookbook(const ItemParameters& parameters)
            : Cookbook(parameters, "item_de_vermis_coctione_cookbook_name", DeVermisCoctioneCookbookMatsetPath)
        {
            SetTooltipDescription("item_de_vermis_coctione_cookbook_description");
            SetTooltipFlavor("item_de_vermis_coctione_cookbook_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_de_vermis_coctione_cookbook");
            AddSelectionSounds(GetBookTrinketSelectionSounds());
        }

        bool DeVermisCoctioneCookbook::CookedFoodHealsOverTime() const
        {
            return true;
        }

        static const std::string TheChefInYellowCookbookMatsetPath = "Skeletons/Items/Trinkets/Cookbook/cookbook_0.polymatset";
        static const uint32_t TheChefInYellowCookbookAdditionalTargetCount = 1;

        TheChefInYellowCookbook::TheChefInYellowCookbook(const ItemParameters& parameters)
            : Cookbook(parameters, "item_the_chef_in_yellow_cookbook_name", TheChefInYellowCookbookMatsetPath)
        {
            SetTooltipDescription("item_the_chef_in_yellow_cookbook_description", TheChefInYellowCookbookAdditionalTargetCount);
            SetTooltipFlavor("item_the_chef_in_yellow_cookbook_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_the_chef_in_yellow_cookbook");
            AddSelectionSounds(GetBookTrinketSelectionSounds());
        }

        uint32_t TheChefInYellowCookbook::GetCookFoodAdditionalTargetCount() const
        {
            return TheChefInYellowCookbookAdditionalTargetCount;
        }

        static const std::string CulinomiconCookbookMatsetPath = "Skeletons/Items/Trinkets/Cookbook/cookbook_1.polymatset";

        CulinomiconCookbook::CulinomiconCookbook(const ItemParameters& parameters)
            : Cookbook(parameters, "item_culinomicon_cookbook_name", CulinomiconCookbookMatsetPath)
        {
            SetTooltipDescription("item_culinomicon_cookbook_description");
            SetTooltipFlavor("item_culinomicon_cookbook_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_culinomicon_cookbook");
            AddSelectionSounds(GetBookTrinketSelectionSounds());
        }

        bool CulinomiconCookbook::CookedFoodTargetsEnemies() const
        {
            return true;
        }
    }

    template <>
    void EnumeratePreloads<Item::Cookbook>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicTrinket>(preloads);

        preloads.insert(Item::CookbookSkeletonPath);
        preloads.insert(Item::CookbookIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::DeVermisCoctioneCookbook>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Cookbook>(preloads);

        preloads.insert(Item::DeVermisCoctioneCookbookMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetBookTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::TheChefInYellowCookbook>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Cookbook>(preloads);

        preloads.insert(Item::TheChefInYellowCookbookMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetBookTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::CulinomiconCookbook>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Cookbook>(preloads);

        preloads.insert(Item::CulinomiconCookbookMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetBookTrinketSelectionSounds());
    }
}
