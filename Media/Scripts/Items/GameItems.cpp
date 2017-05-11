#include "Items/GameItems.hpp"

#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Weapons/Kitchenware.hpp"
#include "Items/Weapons/Hammer.hpp"
#include "Items/Weapons/PickAxe.hpp"
#include "Items/Weapons/Sack.hpp"

#include "Items/Armors/BrewerArmor.hpp"
#include "Items/Armors/BuilderArmor.hpp"
#include "Items/Armors/CookArmor.hpp"
#include "Items/Armors/FighterArmor.hpp"
#include "Items/Armors/MinerArmor.hpp"
#include "Items/Armors/NavigatorArmor.hpp"

#include "Items/Trinkets/Cookbook.hpp"
#include "Items/Trinkets/Pipe.hpp"
#include "Items/Trinkets/HealthReapTrinket.hpp"
#include "Items/Trinkets/ThornsTrinket.hpp"
#include "Items/Trinkets/Headlamp.hpp"

namespace Dwarf
{
    namespace Item
    {
        template <typename TargetType, typename ItemType, typename... argsT>
        GameItem<TargetType> BindGameItem(const std::string& key, argsT... args)
        {
            using namespace std::placeholders;
            return GameItem<TargetType>(key, BindItemConstructor<ItemType, argsT...>(std::forward<argsT>(args)...), EnumeratePreloads<ItemType>);
        }

        // Weapons

        // Brewer
        const GameWeapon Weapons::BasicRifle = BindGameItem<Weapon, Dwarf::Item::BasicRifle>("BasicRifle");
        const GameWeapon Weapons::HandCannonRifle = BindGameItem<Weapon, Dwarf::Item::HandCannonRifle>("HandCannonRifle");
        const GameWeapon Weapons::BlunderbussRifle = BindGameItem<Weapon, Dwarf::Item::BlunderbussRifle>("BlunderbussRifle");
        const GameWeapon Weapons::BlackQueenRifle = BindGameItem<Weapon, Dwarf::Item::BlackQueenRifle>("BlackQueenRifle");

        // Builder
        const GameWeapon Weapons::WoodHammer = BindGameItem<Weapon, Dwarf::Item::WoodHammer>("WoodHammer");
        const GameWeapon Weapons::IronHammer = BindGameItem<Weapon, Dwarf::Item::IronHammer>("IronHammer");
        const GameWeapon Weapons::GoldHammer = BindGameItem<Weapon, Dwarf::Item::GoldHammer>("GoldHammer");

        // Cook
        const GameWeapon Weapons::Cutlery = BindGameItem<Weapon, Dwarf::Item::Cutlery>("Cutlery");
        const GameWeapon Weapons::ServingTools = BindGameItem<Weapon, Dwarf::Item::ServingTools>("ServingTools");
        const GameWeapon Weapons::PrepTools = BindGameItem<Weapon, Dwarf::Item::PrepTools>("PrepTools");
        const GameWeapon Weapons::ButcherTools = BindGameItem<Weapon, Dwarf::Item::ButcherTools>("ButcherTools");
        const GameWeapon Weapons::Cookware = BindGameItem<Weapon, Dwarf::Item::Cookware>("Cookware");

        // Fighter
        const GameWeapon Weapons::WoodenShortSword = BindGameItem<Weapon, Dwarf::Item::WoodenShortSword>("WoodenShortSword");
        const GameWeapon Weapons::RustyIronShortSword = BindGameItem<Weapon, Dwarf::Item::RustyIronShortSword>("RustyIronShortSword");
        const GameWeapon Weapons::IronShortSword = BindGameItem<Weapon, Dwarf::Item::IronShortSword>("IronShortSword");
        const GameWeapon Weapons::IgnitionPinSword = BindGameItem<Weapon, Dwarf::Item::IgnitionPinSword>("IgnitionPinSword");
        const GameWeapon Weapons::LettersbaneSword = BindGameItem<Weapon, Dwarf::Item::LettersbaneSword>("LettersbaneSword");
        const GameWeapon Weapons::UncalculatedDivisionSword = BindGameItem<Weapon, Dwarf::Item::UncalculatedDivisionSword>("UncalculatedDivisionSword");
        const GameWeapon Weapons::WoodenBasicShield = BindGameItem<Weapon, Dwarf::Item::WoodenBasicShield>("WoodenBasicShield");
        const GameWeapon Weapons::WoodenHeater = BindGameItem<Weapon, Dwarf::Item::WoodenHeater>("WoodenHeater");
        const GameWeapon Weapons::BracedWoodenHeater = BindGameItem<Weapon, Dwarf::Item::BracedWoodenHeater>("BracedWoodenHeater");
        const GameWeapon Weapons::IronHeater = BindGameItem<Weapon, Dwarf::Item::IronHeater>("IronHeater");
        const GameWeapon Weapons::WoodenRoundShield = BindGameItem<Weapon, Dwarf::Item::WoodenRoundShield>("WoodenRoundShield");
        const GameWeapon Weapons::TowerShield = BindGameItem<Weapon, Dwarf::Item::TowerShield>("TowerShield");

        // Miner
        const GameWeapon Weapons::WoodPickAxe = BindGameItem<Weapon, Dwarf::Item::WoodPickAxe>("WoodPickAxe");
        const GameWeapon Weapons::IronPickAxe = BindGameItem<Weapon, Dwarf::Item::IronPickAxe>("IronPickAxe");
        const GameWeapon Weapons::GoldPickAxe = BindGameItem<Weapon, Dwarf::Item::GoldPickAxe>("GoldPickAxe");
        const GameWeapon Weapons::DiamondPickAxe = BindGameItem<Weapon, Dwarf::Item::DiamondPickAxe>("DiamondPickAxe");
        const GameWeapon Weapons::Sack = BindGameItem<Weapon, Dwarf::Item::Sack>("Sack");

        // Navigator
        const GameWeapon Weapons::BasicPistol = BindGameItem<Weapon, Dwarf::Item::BasicPistol>("BasicPistol");
        const GameWeapon Weapons::FingerCannonPistol = BindGameItem<Weapon, Dwarf::Item::FingerCannonPistol>("FingerCannonPistol");
        const GameWeapon Weapons::BlunderbussPistol = BindGameItem<Weapon, Dwarf::Item::BlunderbussPistol>("BlunderbussPistol");
        const GameWeapon Weapons::BlackPrincessPistol = BindGameItem<Weapon, Dwarf::Item::BlackPrincessPistol>("BlackPrincessPistol");


        // Armors

        // Brewer
        const GameArmor Armors::BrewerTunic = BindGameItem<Armor, Dwarf::Item::BrewerTunic>("BrewerTunic");

        // Builder
        const GameArmor Armors::BuilderTunic = BindGameItem<Armor, Dwarf::Item::BuilderTunic>("BuilderTunic");

        // Cook
        const GameArmor Armors::Apron = BindGameItem<Armor, Dwarf::Item::Apron>("Apron");

        // Fighter
        const GameArmor Armors::Chainmail = BindGameItem<Armor, Dwarf::Item::Chainmail>("Chainmail");
        const GameArmor Armors::BristleNogginArmor = BindGameItem<Armor, Dwarf::Item::BristleNogginArmor>("BristleNogginArmor");
        const GameArmor Armors::BronzeArmor = BindGameItem<Armor, Dwarf::Item::BronzeArmor>("BronzeArmor");
        const GameArmor Armors::TwinFangArmor = BindGameItem<Armor, Dwarf::Item::TwinFangArmor>("TwinFangArmor");
        const GameArmor Armors::BladeArmor = BindGameItem<Armor, Dwarf::Item::BladeArmor>("BladeArmor");
        const GameArmor Armors::WokArmor = BindGameItem<Armor, Dwarf::Item::WokArmor>("WokArmor");
        const GameArmor Armors::TeapotArmor = BindGameItem<Armor, Dwarf::Item::TeapotArmor>("TeapotArmor");

        // Navigator
        const GameArmor Armors::NavigatorTunic = BindGameItem<Armor, Dwarf::Item::NavigatorTunic>("NavigatorTunic");

        // Miner
        const GameArmor Armors::MinerTunic = BindGameItem<Armor, Dwarf::Item::MinerTunic>("MinerTunic");

        // Trinkets

        // Brewer
        const GameTrinket Trinkets::BriarPipe = BindGameItem<Trinket, Dwarf::Item::BriarPipe>("BriarPipe");
        const GameTrinket Trinkets::MeerschaumPipe = BindGameItem<Trinket, Dwarf::Item::MeerschaumPipe>("MeerschaumPipe");
        const GameTrinket Trinkets::SteinPipe = BindGameItem<Trinket, Dwarf::Item::SteinPipe>("SteinPipe");

        // Cook
        const GameTrinket Trinkets::DeVermisCoctioneCookbook = BindGameItem<Trinket, Dwarf::Item::DeVermisCoctioneCookbook>("DeVermisCoctioneCookbook");
        const GameTrinket Trinkets::TheChefInYellowCookbook = BindGameItem<Trinket, Dwarf::Item::TheChefInYellowCookbook>("TheChefInYellowCookbook");
        const GameTrinket Trinkets::CulinomiconCookbook = BindGameItem<Trinket, Dwarf::Item::CulinomiconCookbook>("CulinomiconCookbook");

        // Fighter
        const GameTrinket Trinkets::ThornsTrinket = BindGameItem<Trinket, Dwarf::Item::ThornsTrinket>("ThornsTrinket", 0.1f);
        const GameTrinket Trinkets::HealthReapTrinket = BindGameItem<Trinket, Dwarf::Item::HealthReapTrinket>("HealthReapTrinket", 0.1f);

        // Navigator
        const GameTrinket Trinkets::BasicHeadlamp = BindGameItem<Trinket, Dwarf::Item::BasicHeadlamp>("BasicHeadlamp");
        const GameTrinket Trinkets::Flashlight = BindGameItem<Trinket, Dwarf::Item::Flashlight>("Flashlight");
        const GameTrinket Trinkets::Lantern = BindGameItem<Trinket, Dwarf::Item::Lantern>("Lantern");
    }

    template<typename GetterFunc>
    static void PreloadFindableItemsPreloads(PreloadSet& preloads, const Item::FindableItems& items, GetterFunc getter)
    {
        for (const auto& classItems : items)
        {
            for (const auto& item : classItems.second)
            {
                getter(item.first)(preloads);
            }
        }
    }

    void EnumerateFindableItemSetPreloads(PreloadSet& preloads, const Item::FindableItemSet& items)
    {
        using namespace std::placeholders;
        PreloadFindableItemsPreloads(preloads, items.Weapons, std::bind(&Item::GameWeapon::GetPreloadFunction, _1));
        PreloadFindableItemsPreloads(preloads, items.Armors, std::bind(&Item::GameArmor::GetPreloadFunction, _1));
        PreloadFindableItemsPreloads(preloads, items.Trinkets, std::bind(&Item::GameTrinket::GetPreloadFunction, _1));
    }
}
