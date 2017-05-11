#pragma once

#include "Item/Item.hpp"
#include "Item/Weapon.hpp"
#include "Item/Armor.hpp"
#include "Item/Trinket.hpp"
#include "Level/LevelInstance.hpp"

#include <string>
#include <map>
#include <functional>

namespace Dwarf
{
    namespace Item
    {
        using FindableItems = std::map<std::string, std::map<std::string, uint32_t>>;
        struct FindableItemSet
        {
            FindableItems Weapons;
            FindableItems Armors;
            FindableItems Trinkets;
        };

        template <typename T, typename... Args>
        FindableItemSet MergeFindableItems(const T& value, Args... args);

        template <typename T>
        struct ItemInfo
        {
            std::string Key;
            ItemConstructor<T> Constructor;
            uint32_t Count;

            ItemInfo();
            ItemInfo(std::string key, ItemConstructor<T> constructor, uint32_t count);
        };
        static const uint32_t InfiniteItemCount = std::numeric_limits<uint32_t>::max();

        typedef ItemInfo<Weapon> WeaponInfo;
        typedef ItemInfo<Armor> ArmorInfo;
        typedef ItemInfo<Trinket> TrinketInfo;

        template <typename T>
        class GameItem
        {
        public:
            GameItem(const std::string& key, ItemConstructor<T> constructor, PreloadFunction preloadFunction);

            const std::string& operator()() const;

            static bool IsItem(const std::string& key);
            static ItemConstructor<T> GetConstructor(const std::string& key);
            static PreloadFunction GetPreloadFunction(const std::string& key);

            static std::vector<std::string> GetAllItems();

        private:
            std::string _key;

            static std::map<std::string, std::pair<ItemConstructor<T>, PreloadFunction>> _items;
        };

        typedef GameItem<Weapon> GameWeapon;
        typedef GameItem<Armor> GameArmor;
        typedef GameItem<Trinket> GameTrinket;

        struct Weapons
        {
            // Brewer
            static const GameWeapon BasicRifle;
            static const GameWeapon HandCannonRifle;
            static const GameWeapon BlunderbussRifle;
            static const GameWeapon BlackQueenRifle;

            // Builder
            static const GameWeapon WoodHammer;
            static const GameWeapon IronHammer;
            static const GameWeapon GoldHammer;

            // Cook
            static const GameWeapon Cutlery;
            static const GameWeapon ServingTools;
            static const GameWeapon PrepTools;
            static const GameWeapon ButcherTools;
            static const GameWeapon Cookware;

            // Fighter
            static const GameWeapon WoodenShortSword;
            static const GameWeapon RustyIronShortSword;
            static const GameWeapon IronShortSword;
            static const GameWeapon IgnitionPinSword;
            static const GameWeapon LettersbaneSword;
            static const GameWeapon UncalculatedDivisionSword;
            static const GameWeapon WoodenBasicShield;
            static const GameWeapon WoodenHeater;
            static const GameWeapon BracedWoodenHeater;
            static const GameWeapon IronHeater;
            static const GameWeapon WoodenRoundShield;
            static const GameWeapon TowerShield;

            // Miner
            static const GameWeapon WoodPickAxe;
            static const GameWeapon IronPickAxe;
            static const GameWeapon GoldPickAxe;
            static const GameWeapon DiamondPickAxe;
            static const GameWeapon Sack;

            // Navigator
            static const GameWeapon BasicPistol;
            static const GameWeapon FingerCannonPistol;
            static const GameWeapon BlunderbussPistol;
            static const GameWeapon BlackPrincessPistol;
        };

        struct Armors
        {
            // Brewer
            static const GameArmor BrewerTunic;

            // Builder
            static const GameArmor BuilderTunic;

            // Cook
            static const GameArmor Apron;

            // Fighter
            static const GameArmor Chainmail;
            static const GameArmor BristleNogginArmor;
            static const GameArmor BronzeArmor;
            static const GameArmor TwinFangArmor;
            static const GameArmor BladeArmor;
            static const GameArmor WokArmor;
            static const GameArmor TeapotArmor;

            // Miner
            static const GameArmor MinerTunic;

            // Navigator
            static const GameArmor NavigatorTunic;
        };

        struct Trinkets
        {
            // Brewer
            static const GameTrinket BriarPipe;
            static const GameTrinket MeerschaumPipe;
            static const GameTrinket SteinPipe;

            // Cook
            static const GameTrinket DeVermisCoctioneCookbook;
            static const GameTrinket TheChefInYellowCookbook;
            static const GameTrinket CulinomiconCookbook;

            // Fighter
            static const GameTrinket ThornsTrinket;
            static const GameTrinket HealthReapTrinket;

            // Navigator
            static const GameTrinket BasicHeadlamp;
            static const GameTrinket Flashlight;
            static const GameTrinket Lantern;
        };
    }

    void EnumerateFindableItemSetPreloads(PreloadSet& preloads, const Item::FindableItemSet& items);
}

#include "GameItems.inl"
