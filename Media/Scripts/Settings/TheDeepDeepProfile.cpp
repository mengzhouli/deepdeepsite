#include "Settings/TheDeepDeepProfile.hpp"

#include "DwarfNameGenerator.hpp"

namespace Dwarf
{
    namespace Settings
    {
        TheDeepDeepProfile::TheDeepDeepProfile()
            : _characterProfiles()
            , _unlockedCampaignLevels()
            , _unlockedChallengeLevels()
            , _cheatsEnabled(false)
        {
            auto allCharacters = Character::GameCharacter::GetAllCharacters();
            for (auto characterType : allCharacters)
            {
                _characterProfiles[characterType] = DwarfTypeProfile();
            }
        }

        TheDeepDeepProfile::DwarfTypeProfile::DwarfTypeProfile()
            : Unlocked(false)
        {
        }

        template <typename T>
        static void addItem(std::vector<Item::ItemInfo<T>>& items, const std::string& item, uint32_t count)
        {
            if (!Item::GameItem<T>::IsItem(item))
            {
                return;
            }

            for (uint32_t i = 0; i < items.size(); i++)
            {
                if (items[i].Key == item)
                {
                    if (items[i].Count != Item::InfiniteItemCount)
                    {
                        if (count == Item::InfiniteItemCount)
                        {
                            items[i].Count = Item::InfiniteItemCount;
                        }
                        else
                        {
                            items[i].Count += count;
                        }
                    }
                    return;
                }
            }

            items.push_back(Item::ItemInfo<T>(item, Item::GameItem<T>::GetConstructor(item), count));
        }

        template <typename T>
        static uint32_t getItemCount(const std::vector<Item::ItemInfo<T>>& items, const std::string& item)
        {
            if (!Item::GameItem<T>::IsItem(item))
            {
                return 0;
            }

            for (uint32_t i = 0; i < items.size(); i++)
            {
                if (items[i].Key == item)
                {
                    return items[i].Count;
                }
            }

            return 0;
        }

        void TheDeepDeepProfile::AddWeapon(const std::string& type, const std::string& item, uint32_t count)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            addItem(_characterProfiles.at(type).Weapons, item, count);
        }

        const Item::ItemInfo<Item::Weapon>& TheDeepDeepProfile::GetWeaponInfo(const std::string& type, uint32_t index) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Weapons[index];
        }

        const std::vector<Item::ItemInfo<Item::Weapon>>& TheDeepDeepProfile::GetWeaponInfo(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Weapons;
        }

        const uint32_t TheDeepDeepProfile::GetWeaponCount(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Weapons.size();
        }

        const uint32_t TheDeepDeepProfile::GetWeaponTypeCount(const std::string& type, const std::string& item) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return getItemCount(_characterProfiles.at(type).Weapons, item);
        }

        void TheDeepDeepProfile::AddArmor(const std::string& type, const std::string& item, uint32_t count)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            addItem(_characterProfiles.at(type).Armors, item, count);
        }

        const Item::ItemInfo<Item::Armor>& TheDeepDeepProfile::GetArmorInfo(const std::string& type, uint32_t index) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Armors[index];
        }

        const std::vector<Item::ItemInfo<Item::Armor>>& TheDeepDeepProfile::GetArmorInfo(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Armors;
        }

        const uint32_t TheDeepDeepProfile::GetArmorCount(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Armors.size();
        }

        const uint32_t TheDeepDeepProfile::GetArmorTypeCount(const std::string& type, const std::string& item) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return getItemCount(_characterProfiles.at(type).Armors, item);
        }

        void TheDeepDeepProfile::AddTrinket(const std::string& type, const std::string& item, uint32_t count)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            addItem(_characterProfiles.at(type).Trinkets, item, count);
        }

        const Item::ItemInfo<Item::Trinket>& TheDeepDeepProfile::GetTrinketInfo(const std::string& type, uint32_t index) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Trinkets[index];
        }

        const std::vector<Item::ItemInfo<Item::Trinket>>& TheDeepDeepProfile::GetTrinketInfo(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Trinkets;
        }

        const uint32_t TheDeepDeepProfile::GetTrinketCount(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Trinkets.size();
        }

        const uint32_t TheDeepDeepProfile::GetTrinketTypeCount(const std::string& type, const std::string& item) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return getItemCount(_characterProfiles.at(type).Trinkets, item);
        }

        void TheDeepDeepProfile::AddAbility(const std::string& type, const std::string& ability)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            DwarfTypeProfile& typeProfile = _characterProfiles.at(type);
            for (uint32_t i = 0; i < typeProfile.Abilities.size(); i++)
            {
                if (typeProfile.Abilities[i].Key == ability)
                {
                    return;
                }
            }

            typeProfile.Abilities.push_back(Ability::AbilityInfo(ability, Ability::GameAbility::GetConstructor(ability)));
        }

        const Ability::AbilityInfo& TheDeepDeepProfile::GetAbilityInfo(const std::string& type, uint32_t index) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Abilities[index];
        }

        const std::vector<Ability::AbilityInfo>& TheDeepDeepProfile::GetAbilityInfo(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Abilities;
        }

        const uint32_t TheDeepDeepProfile::GetAbilityCount(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Abilities.size();
        }

        const uint32_t TheDeepDeepProfile::IsAbilityAdded(const std::string& type, const std::string& ability) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return std::any_of(_characterProfiles.at(type).Abilities.begin(), _characterProfiles.at(type).Abilities.end(), [&](const Ability::AbilityInfo& info)
            {
                return ability == info.Key;
            });
        }

        bool TheDeepDeepProfile::IsCharacterTypeUnlocked(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Unlocked;
        }

        void TheDeepDeepProfile::UnlockCharacterType(const std::string& type)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            _characterProfiles.at(type).Unlocked = true;
        }

        void TheDeepDeepProfile::GenerateNewDwarfInfo(const std::string& type)
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());

            Character::DwarfInfo newInfo;
            newInfo.Name = Character::GenerateDwarfFullName(type);
            newInfo.Size = Random::RandomBetween(0.925f, 1.075f);

            _characterProfiles.at(type).Available.push_back(newInfo);
        }

        void TheDeepDeepProfile::SetDwarfDeceased(const std::string& name)
        {
            for (DwarfProfileMap::iterator i = _characterProfiles.begin(); i != _characterProfiles.end(); i++)
            {
                DwarfTypeProfile& profile = i->second;
                for (std::vector<Character::DwarfInfo>::iterator j = profile.Available.begin(); j != profile.Available.end(); j++)
                {
                    if (j->Name == name)
                    {
                        profile.Deceased.push_back(*j);
                        profile.Available.erase(j);
                        return;
                    }
                }
            }
        }

        const Character::DwarfInfo& TheDeepDeepProfile::GetCharacterInfo(const std::string& name) const
        {
            Character::DwarfInfo *info = nullptr;
            bool ret = getDwarfInfoByName(name, &info, nullptr);
            assert(ret && info);
            (void)ret;
            return *info;
        }

        void TheDeepDeepProfile::UpdateCharacterInfo(const std::string& name, const Character::DwarfInfo& profile)
        {
            Character::DwarfInfo *info = nullptr;
            bool ret = getDwarfInfoByName(name, &info, nullptr);
            assert(ret && info);
            (void)ret;
            *info = profile;
        }

        bool TheDeepDeepProfile::HasCharacterInfo(const std::string& name)
        {
            return getDwarfInfoByName(name, nullptr, nullptr);
        }

        const Character::DwarfInfo& TheDeepDeepProfile::GetCharacterInfo(const std::string& type, uint32_t index) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Available[index];
        }

        uint32_t TheDeepDeepProfile::GetCharacterInfoCount(const std::string& type) const
        {
            assert(_characterProfiles.find(type) != _characterProfiles.end());
            return _characterProfiles.at(type).Available.size();
        }

        bool TheDeepDeepProfile::IsCampaignLevelUnlocked(const std::string& key) const
        {
            return std::find(_unlockedCampaignLevels.begin(), _unlockedCampaignLevels.end(), key) != _unlockedCampaignLevels.end();
        }

        bool TheDeepDeepProfile::IsCampaignLevelJustUnlocked(const std::string& key) const
        {
            return IsCampaignLevelUnlocked(key) &&
                  std::find(_justUnlockedCampaignLevels.begin(), _justUnlockedCampaignLevels.end(), key) != _justUnlockedCampaignLevels.end();
        }

        void TheDeepDeepProfile::UnlockCampaignLevel(const std::string& key)
        {
            _unlockedCampaignLevels.insert(key);
            _justUnlockedCampaignLevels.insert(key);
        }

        void TheDeepDeepProfile::MarkCampaignLevelSeen(const std::string& key)
        {
            _justUnlockedCampaignLevels.erase(key);
        }

        bool TheDeepDeepProfile::IsChallengeLevelUnlocked(const std::string& key) const
        {
            return std::find(_unlockedChallengeLevels.begin(), _unlockedChallengeLevels.end(), key) != _unlockedChallengeLevels.end();
        }

        bool TheDeepDeepProfile::IsChallengeLevelJustUnlocked(const std::string& key) const
        {
            return IsChallengeLevelUnlocked(key) &&
                std::find(_justUnlockedChallengeLevels.begin(), _justUnlockedChallengeLevels.end(), key) != _justUnlockedChallengeLevels.end();
        }

        void TheDeepDeepProfile::UnlockChallengeLevel(const std::string& key)
        {
            _unlockedChallengeLevels.insert(key);
            _justUnlockedChallengeLevels.insert(key);
        }

        void TheDeepDeepProfile::MarkChallengeLevelSeen(const std::string& key)
        {
            _justUnlockedChallengeLevels.erase(key);
        }

        bool TheDeepDeepProfile::HasTutorialBeenShown(TutorialType tutorial)
        {
            return _shownTutorials.find(tutorial) != _shownTutorials.end();
        }

        void TheDeepDeepProfile::MarkTutorialShown(TutorialType tutorial)
        {
            _shownTutorials.insert(tutorial);
        }

        template <typename T>
        static void EnumerateItemTypePreloads(const std::vector<Item::ItemInfo<T>>& infos, PreloadSet& preloads)
        {
            for (auto info : infos)
            {
                PreloadFunction func = Item::GameItem<T>::GetPreloadFunction(info.Key);
                func(preloads);
            }
        }

        void TheDeepDeepProfile::EnumerateItemPreloads(PreloadSet& preloads) const
        {
            for (auto charType : _characterProfiles)
            {
                EnumerateItemTypePreloads(charType.second.Weapons, preloads);
                EnumerateItemTypePreloads(charType.second.Armors, preloads);
                EnumerateItemTypePreloads(charType.second.Trinkets, preloads);
            }
        }

        void TheDeepDeepProfile::EnumerateAbilityPreloads(PreloadSet& preloads) const
        {
            for (auto charType : _characterProfiles)
            {
                for (auto info : charType.second.Abilities)
                {
                    PreloadFunction func = Ability::GameAbility::GetPreloadFunction(info.Key);
                    func(preloads);
                }
            }
        }

        void TheDeepDeepProfile::UnlockEverything()
        {
            UnlockCharacterType(Character::Dwarves::Brewer());
            AddWeapon(Character::Dwarves::Brewer(), Item::Weapons::BasicRifle(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Brewer(), Item::Weapons::HandCannonRifle(), 1);
            AddWeapon(Character::Dwarves::Brewer(), Item::Weapons::BlunderbussRifle(), 1);
            AddWeapon(Character::Dwarves::Brewer(), Item::Weapons::BlackQueenRifle(), 1);
            AddTrinket(Character::Dwarves::Brewer(), Item::Trinkets::BriarPipe(), 1);
            AddTrinket(Character::Dwarves::Brewer(), Item::Trinkets::MeerschaumPipe(), 1);
            AddTrinket(Character::Dwarves::Brewer(), Item::Trinkets::SteinPipe(), 1);
            AddAbility(Character::Dwarves::Brewer(), Ability::Abilities::ThrowBomb());
            AddAbility(Character::Dwarves::Brewer(), Ability::Abilities::PlantDynamite());

            UnlockCharacterType(Character::Dwarves::Builder());
            AddWeapon(Character::Dwarves::Builder(), Item::Weapons::WoodHammer(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Builder(), Item::Weapons::IronHammer(), 1);
            AddWeapon(Character::Dwarves::Builder(), Item::Weapons::GoldHammer(), 1);
            AddAbility(Character::Dwarves::Builder(), Ability::Abilities::BuildBarricade());
            AddAbility(Character::Dwarves::Builder(), Ability::Abilities::BuildBridge());

            UnlockCharacterType(Character::Dwarves::Cook());
            AddWeapon(Character::Dwarves::Cook(), Item::Weapons::Cutlery(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Cook(), Item::Weapons::ServingTools(), 1);
            AddWeapon(Character::Dwarves::Cook(), Item::Weapons::PrepTools(), 1);
            AddWeapon(Character::Dwarves::Cook(), Item::Weapons::ButcherTools(), 1);
            AddWeapon(Character::Dwarves::Cook(), Item::Weapons::Cookware(), 1);
            AddTrinket(Character::Dwarves::Cook(), Item::Trinkets::DeVermisCoctioneCookbook(), 1);
            AddTrinket(Character::Dwarves::Cook(), Item::Trinkets::TheChefInYellowCookbook(), 1);
            AddTrinket(Character::Dwarves::Cook(), Item::Trinkets::CulinomiconCookbook(), 1);
            AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookBeef());
            AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookChicken());
            AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookHam());

            UnlockCharacterType(Character::Dwarves::Fighter());
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenShortSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::RustyIronShortSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::IronShortSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::IgnitionPinSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::LettersbaneSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::UncalculatedDivisionSword(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenBasicShield(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenHeater(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::BracedWoodenHeater(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::IronHeater(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenRoundShield(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::TowerShield(), Item::InfiniteItemCount);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::BristleNogginArmor(), 1);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::BronzeArmor(), 1);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::TwinFangArmor(), 1);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::BladeArmor(), 1);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::WokArmor(), 1);
            AddArmor(Character::Dwarves::Fighter(), Item::Armors::TeapotArmor(), 1);
            AddAbility(Character::Dwarves::Fighter(), Ability::Abilities::Leap());

            UnlockCharacterType(Character::Dwarves::Miner());
            AddWeapon(Character::Dwarves::Miner(), Item::Weapons::WoodPickAxe(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Miner(), Item::Weapons::Sack(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Miner(), Item::Weapons::IronPickAxe(), 1);
            AddWeapon(Character::Dwarves::Miner(), Item::Weapons::GoldPickAxe(), 1);
            AddWeapon(Character::Dwarves::Miner(), Item::Weapons::DiamondPickAxe(), 1);

            UnlockCharacterType(Character::Dwarves::Navigator());
            AddWeapon(Character::Dwarves::Navigator(), Item::Weapons::BasicPistol(), Item::InfiniteItemCount);
            AddWeapon(Character::Dwarves::Navigator(), Item::Weapons::FingerCannonPistol(), 1);
            AddWeapon(Character::Dwarves::Navigator(), Item::Weapons::BlunderbussPistol(), 1);
            AddWeapon(Character::Dwarves::Navigator(), Item::Weapons::BlackPrincessPistol(), 1);
            AddTrinket(Character::Dwarves::Navigator(), Item::Trinkets::BasicHeadlamp(), Item::InfiniteItemCount);
            AddTrinket(Character::Dwarves::Navigator(), Item::Trinkets::Flashlight(), 1);
            AddTrinket(Character::Dwarves::Navigator(), Item::Trinkets::Lantern(), 1);
            AddAbility(Character::Dwarves::Navigator(), Ability::Abilities::Grapple());
            AddAbility(Character::Dwarves::Navigator(), Ability::Abilities::ShootFlare());

            UnlockCampaignLevel(Level::Campaign::DwarfHomes0());
            UnlockCampaignLevel(Level::Campaign::DwarfHomes1());
            UnlockCampaignLevel(Level::Campaign::DwarfHomes2());
            UnlockCampaignLevel(Level::Campaign::DwarfHomes3());
            UnlockCampaignLevel(Level::Campaign::DwarfHomes4());
        }

        void TheDeepDeepProfile::SetCheatsEnabled(bool enabled)
        {
            _cheatsEnabled = enabled;
            if (_cheatsEnabled)
            {
                UnlockEverything();
            }
        }

        bool TheDeepDeepProfile::AreCheatsEnabled() const
        {
            return _cheatsEnabled;
        }

        std::string TheDeepDeepProfile::GetDefaultProfilePath()
        {
            return Format("%s/%s/%s", FileSystem::GetHomePath().c_str(), "Dwarf", "settings.config");
        }

        bool TheDeepDeepProfile::getDwarfInfoByName(const std::string& name, Character::DwarfInfo** outInfo, DwarfTypeProfile** outProfile) const
        {
            for (DwarfProfileMap::const_iterator i = _characterProfiles.begin(); i != _characterProfiles.end(); i++)
            {
                const DwarfTypeProfile& profile = i->second;
                for (std::vector<Character::DwarfInfo>::const_iterator j = profile.Available.begin(); j != profile.Available.end(); j++)
                {
                    if (j->Name == name)
                    {
                        if (outInfo)
                        {
                            *outInfo = (const_cast<Character::DwarfInfo*>(&(*j)));
                        }

                        if (outProfile)
                        {
                            *outProfile = const_cast<DwarfTypeProfile*>(&profile);
                        }

                        return true;
                    }
                }
            }

            return false;
        }


        void WriteToXML(XML::XMLNode& node, const TheDeepDeepProfile& value)
        {
            node.SetValue(static_cast<const GameProfile&>(value));

            {
                XML::XMLNode dwarvesNode = node.AddChild("Dwarves");
                for (auto characterProfile : value._characterProfiles)
                {
                    const TheDeepDeepProfile::DwarfTypeProfile& typeProfile = characterProfile.second;
                    XML::XMLNode dwarfTypeNode = dwarvesNode.AddChild(characterProfile.first);

                    dwarfTypeNode.AddChild("Unlocked").SetValue(typeProfile.Unlocked);

                    {
                        XML::XMLNode availableDwarvesNode = dwarfTypeNode.AddChild("Available");
                        for (uint32_t j = 0; j < typeProfile.Available.size(); j++)
                        {
                            XML::XMLNode dwarfNode = availableDwarvesNode.AddChild("Dwarf");
                            WriteToXML(dwarfNode, typeProfile.Available[j]);
                        }
                    }

                    {
                        XML::XMLNode deceasedDwarvesNode = dwarfTypeNode.AddChild("Deceased");
                        for (uint32_t j = 0; j < typeProfile.Deceased.size(); j++)
                        {
                            XML::XMLNode dwarfNode = deceasedDwarvesNode.AddChild("Dwarf");
                            WriteToXML(dwarfNode, typeProfile.Deceased[j]);
                        }
                    }

                    {
                        XML::XMLNode weaponsNode = dwarfTypeNode.AddChild("Weapons");
                        for (uint32_t j = 0; j < typeProfile.Weapons.size(); j++)
                        {
                            XML::XMLNode weaponNode = weaponsNode.AddChild("Weapon");
                            weaponNode.AddChild("Name").SetValue(typeProfile.Weapons[j].Key);
                            weaponNode.AddChild("Count").SetValue(typeProfile.Weapons[j].Count);
                        }
                    }

                    {
                        XML::XMLNode armorsNode = dwarfTypeNode.AddChild("Armors");
                        for (uint32_t j = 0; j < typeProfile.Armors.size(); j++)
                        {
                            XML::XMLNode armorNode = armorsNode.AddChild("Armor");
                            armorNode.AddChild("Name").SetValue(typeProfile.Armors[j].Key);
                            armorNode.AddChild("Count").SetValue(typeProfile.Armors[j].Count);
                        }
                    }

                    {
                        XML::XMLNode trinketsNode = dwarfTypeNode.AddChild("Trinkets");
                        for (uint32_t j = 0; j < typeProfile.Trinkets.size(); j++)
                        {
                            XML::XMLNode trinketNode = trinketsNode.AddChild("Trinket");
                            trinketNode.AddChild("Name").SetValue(typeProfile.Trinkets[j].Key);
                            trinketNode.AddChild("Count").SetValue(typeProfile.Trinkets[j].Count);
                        }
                    }

                    {
                        XML::XMLNode abilitiesNode = dwarfTypeNode.AddChild("Abilities");
                        for (uint32_t j = 0; j < typeProfile.Abilities.size(); j++)
                        {
                            XML::XMLNode abilityNode = abilitiesNode.AddChild("Ability");
                            abilityNode.AddChild("Name").SetValue(typeProfile.Abilities[j].Key);
                        }
                    }
                }
            }

            {
                XML::XMLNode campaignLevelsNode = node.AddChild("CampaignLevels");
                for (const auto& campaignLevel : value._unlockedCampaignLevels)
                {
                    XML::XMLNode campaignLevelNode = campaignLevelsNode.AddChild("CampaignLevel");
                    campaignLevelNode.AddChild("Name").SetValue(campaignLevel);
                    campaignLevelNode.AddChild("Unlocked").SetValue(true);
                    campaignLevelNode.AddChild("JustUnlocked").SetValue(value.IsCampaignLevelJustUnlocked(campaignLevel));
                }
            }


            {
                XML::XMLNode challengeLevelsNode = node.AddChild("ChallengeLevels");
                for (const auto& challengeLevel : value._unlockedChallengeLevels)
                {
                    XML::XMLNode challengeLevelNode = challengeLevelsNode.AddChild("ChallengeLevel");
                    challengeLevelsNode.AddChild("Name").SetValue(challengeLevel);
                    challengeLevelsNode.AddChild("Unlocked").SetValue(true);
                    challengeLevelsNode.AddChild("JustUnlocked").SetValue(value.IsChallengeLevelJustUnlocked(challengeLevel));
                }
            }

            {
                XML::XMLNode tutorialsShownNode = node.AddChild("TutorialsShown");
                for (TutorialType tutorialShown : value._shownTutorials)
                {
                    tutorialsShownNode.AddChild("Tutorial").SetValue(tutorialShown);
                }
            }

            node.AddChild("Cheats").SetValue(value.AreCheatsEnabled());
        }

        void ReadFromXML(const XML::XMLNode& node, TheDeepDeepProfile& value)
        {
            static_cast<GameProfile&>(value) = node.GetValue<GameProfile>();

            const TheDeepDeepProfile& defaultProfile = GetDefaultTheDeepDeepProfile();

            {
                for (uint32_t i = 0; i < defaultProfile.GetInputBindCount(); i++)
                {
                    const Input::InputBinding& defaultBind = defaultProfile.GetBindByIndex(i);

                    if (value.GetBindCode(defaultBind.Name) == Input::InputBindCode_Invalid)
                    {
                        value.SetBind(defaultBind);
                    }
                }
            }

            {
                value._characterProfiles = defaultProfile._characterProfiles;
                XML::XMLNode dwarvesNode = node.GetChild("Dwarves");
                for (auto characterProfile : value._characterProfiles)
                {
                    const std::string& dwarfType = characterProfile.first;
                    XML::XMLNode dwarfTypeNode = dwarvesNode.GetChild(dwarfType);

                    TheDeepDeepProfile::DwarfTypeProfile& typeProfile = value._characterProfiles[dwarfType];
                    typeProfile.Unlocked = dwarfTypeNode.GetChild("Unlocked").GetValue<bool>();

                    XML::XMLNode availableDwarvesNode = dwarfTypeNode.GetChild("Available");
                    for (XML::XMLNode dwarfNode = availableDwarvesNode.GetChild("Dwarf"); dwarfNode; dwarfNode = dwarfNode.GetNextSibling("Dwarf"))
                    {
                        typeProfile.Available.push_back(dwarfNode.GetValue<Character::DwarfInfo>());
                    }

                    XML::XMLNode deceasedDwarvesNode = dwarfTypeNode.GetChild("Deceased");
                    for (XML::XMLNode dwarfNode = deceasedDwarvesNode.GetChild("Dwarf"); dwarfNode; dwarfNode = dwarfNode.GetNextSibling("Dwarf"))
                    {
                        typeProfile.Deceased.push_back(dwarfNode.GetValue<Character::DwarfInfo>());
                    }

                    XML::XMLNode weaponsNode = dwarfTypeNode.GetChild("Weapons");
                    for (XML::XMLNode weaponNode = weaponsNode.GetChild("Weapon"); weaponNode; weaponNode = weaponNode.GetNextSibling("Weapon"))
                    {
                        std::string key = weaponNode.GetChild("Name").GetValue<std::string>();
                        uint32_t count = weaponNode.GetChild("Count").GetValue<uint32_t>();
                        value.AddWeapon(dwarfType, key, count);
                    }

                    XML::XMLNode armorsNode = dwarfTypeNode.GetChild("Armors");
                    for (XML::XMLNode armorNode = armorsNode.GetChild("Armor"); armorNode; armorNode = armorNode.GetNextSibling("Armor"))
                    {
                        std::string key = armorNode.GetChild("Name").GetValue<std::string>();
                        uint32_t count = armorNode.GetChild("Count").GetValue<uint32_t>();
                        value.AddArmor(dwarfType, key, count);
                    }

                    XML::XMLNode trinketsNode = dwarfTypeNode.GetChild("Trinkets");
                    for (XML::XMLNode trinketNode = trinketsNode.GetChild("Trinket"); trinketNode; trinketNode = trinketNode.GetNextSibling("Trinket"))
                    {
                        std::string key = trinketNode.GetChild("Name").GetValue<std::string>();
                        uint32_t count = trinketNode.GetChild("Count").GetValue<uint32_t>();
                        value.AddTrinket(dwarfType, key, count);
                    }

                    XML::XMLNode abilitiesNode = dwarfTypeNode.GetChild("Abilities");
                    for (XML::XMLNode abilityNode = abilitiesNode.GetChild("Ability"); abilityNode; abilityNode = abilityNode.GetNextSibling("Ability"))
                    {
                        std::string key = abilityNode.GetChild("Name").GetValue<std::string>();
                        value.AddAbility(dwarfType, key);
                    }
                }
            }

            {
                value._unlockedCampaignLevels = defaultProfile._unlockedCampaignLevels;
                XML::XMLNode campaignLevelsNode = node.GetChild("CampaignLevels");
                for (XML::XMLNode campaignLevelNode = campaignLevelsNode.GetChild("CampaignLevel"); campaignLevelNode; campaignLevelNode = campaignLevelNode.GetNextSibling("CampaignLevel"))
                {
                    std::string key = campaignLevelNode.GetChild("Name").GetValue<std::string>();
                    bool unlocked = campaignLevelNode.GetChild("Unlocked").GetValue<bool>();
                    bool justUnlocked = campaignLevelNode.GetChild("JustUnlocked").GetValue<bool>();

                    if (unlocked)
                    {
                        value.UnlockCampaignLevel(key);
                        if (!justUnlocked)
                        {
                            value.MarkCampaignLevelSeen(key);
                        }
                    }
                }
            }

            {
                value._unlockedChallengeLevels = defaultProfile._unlockedChallengeLevels;
                XML::XMLNode challengeLevelsNode = node.GetChild("ChallengeLevels");
                for (XML::XMLNode challengeLevelNode = challengeLevelsNode.GetChild("ChallengeLevel"); challengeLevelNode; challengeLevelNode = challengeLevelNode.GetNextSibling("ChallengeLevel"))
                {
                    std::string key = challengeLevelNode.GetChild("Name").GetValue<std::string>();
                    bool unlocked = challengeLevelNode.GetChild("Unlocked").GetValue<bool>();
                    bool justUnlocked = challengeLevelNode.GetChild("JustUnlocked").GetValue<bool>();

                    if (unlocked)
                    {
                        value.UnlockChallengeLevel(key);
                        if (!justUnlocked)
                        {
                            value.MarkCampaignLevelSeen(key);
                        }
                    }
                }
            }

            {
                value._shownTutorials = defaultProfile._shownTutorials;
                XML::XMLNode tutorialsShownNode = node.GetChild("TutorialsShown");
                for (XML::XMLNode tutorialShownNode = tutorialsShownNode.GetChild("Tutorial"); tutorialShownNode; tutorialShownNode = tutorialShownNode.GetNextSibling("Tutorial"))
                {
                    TutorialType tutorial = tutorialShownNode.GetValue<TutorialType>();
                    value.MarkTutorialShown(tutorial);
                }
            }

            value.SetCheatsEnabled(node.GetChild("Cheats").GetValue<bool>(false));
        }

        static TheDeepDeepProfile generateDefaultProfile()
        {
            TheDeepDeepProfile profile;

            profile.SetName("Default");

            profile.SetBind("hud_select", Input::Mouse::MouseButton_Left);

            profile.SetBind("select", Input::Mouse::MouseButton_Left);
            profile.SetBind("action", Input::Mouse::MouseButton_Right);

            profile.SetBind("debug_ui", Input::Keyboard::Key_K);
            profile.SetBind("debug_0", Input::Keyboard::Key_F10);
            profile.SetBind("debug_1", Input::Keyboard::Key_F1);
            profile.SetBind("debug_2", Input::Keyboard::Key_F2);
            profile.SetBind("debug_3", Input::Keyboard::Key_F3);
            profile.SetBind("debug_4", Input::Keyboard::Key_F4);
            profile.SetBind("debug_5", Input::Keyboard::Key_F5);
            profile.SetBind("debug_6", Input::Keyboard::Key_F6);
            profile.SetBind("debug_7", Input::Keyboard::Key_F7);
            profile.SetBind("debug_8", Input::Keyboard::Key_F8);
            profile.SetBind("debug_9", Input::Keyboard::Key_F9);

            profile.SetBind("attack", Input::Keyboard::Key_A);
            profile.SetBind("stop", Input::Keyboard::Key_S);
            profile.SetBind("move", Input::Keyboard::Key_M);
            profile.SetBind("hold_position", Input::Keyboard::Key_H);
            profile.SetBind("ability_0", Input::Keyboard::Key_Q);
            profile.SetBind("ability_1", Input::Keyboard::Key_W);
            profile.SetBind("ability_2", Input::Keyboard::Key_E);
            profile.SetBind("ability_3", Input::Keyboard::Key_R);

            profile.SetBind("character_0", Input::Keyboard::Key_Num1);
            profile.SetBind("character_1", Input::Keyboard::Key_Num2);
            profile.SetBind("character_2", Input::Keyboard::Key_Num3);
            profile.SetBind("character_3", Input::Keyboard::Key_Num4);
            profile.SetBind("character_4", Input::Keyboard::Key_Num5);
            profile.SetBind("character_5", Input::Keyboard::Key_Num6);
            profile.SetBind("character_6", Input::Keyboard::Key_Num7);
            profile.SetBind("character_7", Input::Keyboard::Key_Num8);
            profile.SetBind("character_8", Input::Keyboard::Key_Num9);
            profile.SetBind("character_9", Input::Keyboard::Key_Num0);
            profile.SetBind("character_10", Input::Keyboard::Key_Dash);
            profile.SetBind("character_11", Input::Keyboard::Key_Equal);
            profile.SetBind("character_12", Input::Keyboard::Key_Tilde);

            profile.SetBind("queue", Input::Keyboard::Key_LShift);
            profile.SetBind("cancel", Input::Keyboard::Key_Escape);

            profile.SetBind("select_all", Input::Keyboard::Key_Tilde);
            profile.SetBind("iterate_selection", Input::Keyboard::Key_Tab);

            profile.SetBind("up", Input::Keyboard::Key_Up);
            profile.SetBind("down", Input::Keyboard::Key_Down);
            profile.SetBind("left", Input::Keyboard::Key_Left);
            profile.SetBind("right", Input::Keyboard::Key_Right);

            profile.SetBind("menu_up", Input::Keyboard::Key_Up);
            profile.SetBind("menu_down", Input::Keyboard::Key_Down);
            profile.SetBind("menu_left", Input::Keyboard::Key_Left);
            profile.SetBind("menu_right", Input::Keyboard::Key_Right);
            profile.SetBind("menu_select", Input::Mouse::MouseButton_Left);
            profile.SetBind("menu_back", Input::Keyboard::Key_Back);

            profile.SetBind("pause", Input::Keyboard::Key_Space);

            profile.SetLanguage(Localization::Language_English);

            // Initial unlocks
            profile.AddArmor(Character::Dwarves::Brewer(), Item::Armors::BrewerTunic(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Brewer(), Item::Weapons::BasicRifle(), Item::InfiniteItemCount);

            profile.AddArmor(Character::Dwarves::Builder(), Item::Armors::BuilderTunic(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Builder(), Item::Weapons::WoodHammer(), Item::InfiniteItemCount);

            profile.AddArmor(Character::Dwarves::Cook(), Item::Armors::Apron(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Cook(), Item::Weapons::Cutlery(), Item::InfiniteItemCount);
            //profile.AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookBeef());
            //profile.AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookChicken());
            //profile.AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookHam());
            //profile.AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookCritFood());
            //profile.AddAbility(Character::Dwarves::Cook(), Ability::Abilities::CookAttackSpeedFood());

            profile.AddArmor(Character::Dwarves::Fighter(), Item::Armors::Chainmail(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenShortSword(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Fighter(), Item::Weapons::WoodenBasicShield(), Item::InfiniteItemCount);

            profile.AddArmor(Character::Dwarves::Miner(), Item::Armors::MinerTunic(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Miner(), Item::Weapons::WoodPickAxe(), Item::InfiniteItemCount);
            profile.AddWeapon(Character::Dwarves::Miner(), Item::Weapons::Sack(), Item::InfiniteItemCount);
            profile.AddAbility(Character::Dwarves::Miner(), Ability::Abilities::Sprint());

            profile.UnlockCharacterType(Character::Dwarves::Navigator());
            profile.AddWeapon(Character::Dwarves::Navigator(), Item::Weapons::BasicPistol(), Item::InfiniteItemCount);
            profile.AddArmor(Character::Dwarves::Navigator(), Item::Armors::NavigatorTunic(), Item::InfiniteItemCount);
            profile.AddTrinket(Character::Dwarves::Navigator(), Item::Trinkets::BasicHeadlamp(), Item::InfiniteItemCount);
            //profile.AddAbility(Character::Dwarves::Navigator(), Ability::Abilities::ShootFlare());

            profile.UnlockCampaignLevel(Level::Campaign::DwarfHomes0());

            return profile;
        }

        const TheDeepDeepProfile& GetDefaultTheDeepDeepProfile()
        {
            static TheDeepDeepProfile defaultProfile = generateDefaultProfile();
            return defaultProfile;
        }
    }
}
