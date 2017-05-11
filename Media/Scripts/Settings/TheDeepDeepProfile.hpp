#pragma once

#include "Settings/GameProfile.hpp"

#include "Characters/Dwarves/Dwarf.hpp"

#include "Items/GameItems.hpp"
#include "Abilities/GameAbilities.hpp"
#include "Levels/GameLevels.hpp"
#include "Characters/GameCharacters.hpp"
#include "Tutorials.hpp"

namespace Dwarf
{
    namespace Settings
    {
        class TheDeepDeepProfile : public GameProfile
        {
        public:
            TheDeepDeepProfile();

            // Weapons
            void AddWeapon(const std::string& type, const std::string& item, uint32_t count);
            const Item::ItemInfo<Item::Weapon>& GetWeaponInfo(const std::string& type, uint32_t index) const;
            const uint32_t GetWeaponCount(const std::string& type) const;
            const uint32_t GetWeaponTypeCount(const std::string& type, const std::string& item) const;
            const std::vector<Item::ItemInfo<Item::Weapon>>& GetWeaponInfo(const std::string& type) const;

            // Armor
            void AddArmor(const std::string& type, const std::string& item, uint32_t count);
            const Item::ItemInfo<Item::Armor>& GetArmorInfo(const std::string& type, uint32_t index) const;
            const uint32_t GetArmorCount(const std::string& type) const;
            const uint32_t GetArmorTypeCount(const std::string& type, const std::string& item) const;
            const std::vector<Item::ItemInfo<Item::Armor>>& GetArmorInfo(const std::string& type) const;

            // Trinkets
            void AddTrinket(const std::string& type, const std::string& item, uint32_t count);
            const Item::ItemInfo<Item::Trinket>& GetTrinketInfo(const std::string& type, uint32_t index) const;
            const uint32_t GetTrinketCount(const std::string& type) const;
            const uint32_t GetTrinketTypeCount(const std::string& type, const std::string& item) const;
            const std::vector<Item::ItemInfo<Item::Trinket>>& GetTrinketInfo(const std::string& type) const;

            // Abilities
            void AddAbility(const std::string& type, const std::string& ability);
            const Ability::AbilityInfo& GetAbilityInfo(const std::string& type, uint32_t index) const;
            const uint32_t GetAbilityCount(const std::string& type) const;
            const uint32_t IsAbilityAdded(const std::string& type, const std::string& ability) const;
            const std::vector<Ability::AbilityInfo>& GetAbilityInfo(const std::string& type) const;

            // Characters
            bool IsCharacterTypeUnlocked(const std::string& type) const;
            void UnlockCharacterType(const std::string& type);

            void GenerateNewDwarfInfo(const std::string& type);
            void SetDwarfDeceased(const std::string& name);

            const Character::DwarfInfo& GetCharacterInfo(const std::string& name) const;
            void UpdateCharacterInfo(const std::string& name, const Character::DwarfInfo& profile);
            bool HasCharacterInfo(const std::string& name);

            const Character::DwarfInfo& GetCharacterInfo(const std::string& type, uint32_t index) const;
            uint32_t GetCharacterInfoCount(const std::string& type) const;

            // Levels
            bool IsCampaignLevelUnlocked(const std::string& key) const;
            bool IsCampaignLevelJustUnlocked(const std::string& key) const;
            void UnlockCampaignLevel(const std::string& key);
            void MarkCampaignLevelSeen(const std::string& key);

            bool IsChallengeLevelUnlocked(const std::string& key) const;
            bool IsChallengeLevelJustUnlocked(const std::string& key) const;
            void UnlockChallengeLevel(const std::string& key);
            void MarkChallengeLevelSeen(const std::string& key);

            // Tutorials
            bool HasTutorialBeenShown(TutorialType tutorial);
            void MarkTutorialShown(TutorialType tutorial);

            // Preloads
            void EnumerateItemPreloads(PreloadSet& preloads) const;
            void EnumerateAbilityPreloads(PreloadSet& preloads) const;

            // Utilities
            void UnlockEverything();
            void SetCheatsEnabled(bool enabled);
            bool AreCheatsEnabled() const;

            static std::string GetDefaultProfilePath();

        private:
            struct DwarfTypeProfile
            {
                bool Unlocked;

                std::vector<Character::DwarfInfo> Available;
                std::vector<Character::DwarfInfo> Deceased;

                std::vector<Item::ItemInfo<Item::Weapon>> Weapons;
                std::vector<Item::ItemInfo<Item::Armor>> Armors;
                std::vector<Item::ItemInfo<Item::Trinket>> Trinkets;

                std::vector<Ability::AbilityInfo> Abilities;

                DwarfTypeProfile();
            };

            bool getDwarfInfoByName(const std::string& name, Character::DwarfInfo** outInfo, DwarfTypeProfile** outProfile) const;

            typedef std::map<std::string, DwarfTypeProfile> DwarfProfileMap;
            DwarfProfileMap _characterProfiles;

            std::set<std::string> _unlockedCampaignLevels;
            std::set<std::string> _justUnlockedCampaignLevels;
            std::set<std::string> _unlockedChallengeLevels;
            std::set<std::string> _justUnlockedChallengeLevels;
            std::set<TutorialType> _shownTutorials;

            bool _cheatsEnabled;

            friend void WriteToXML(XML::XMLNode& node, const TheDeepDeepProfile& value);
            friend void ReadFromXML(const XML::XMLNode& node, TheDeepDeepProfile& value);
        };

        void WriteToXML(XML::XMLNode& node, const TheDeepDeepProfile& value);
        void ReadFromXML(const XML::XMLNode& node, TheDeepDeepProfile& value);

        const TheDeepDeepProfile& GetDefaultTheDeepDeepProfile();
    }
}
