#include "Abilities/GameAbilities.hpp"

#include "Abilities/ThrowBomb.hpp"
#include "Abilities/PlantDynamite.hpp"
#include "Abilities/Brew.hpp"
#include "Abilities/BuildBarricade.hpp"
#include "Abilities/BuildBridge.hpp"
#include "Abilities/CookFood.hpp"
#include "Abilities/ChannelingFoodHeal.hpp"
#include "Abilities/Sprint.hpp"
#include "Abilities/Leap.hpp"
#include "Abilities/Grapple.hpp"
#include "Abilities/ShootFlare.hpp"
#include "Abilities/SackSmash.hpp"

namespace Dwarf
{
    namespace Ability
    {
        AbilityInfo::AbilityInfo()
            : AbilityInfo("", AbilityConstructor<>())
        {
        }

        AbilityInfo::AbilityInfo(std::string key, AbilityConstructor<> constructor)
            : Key(key)
            , Constructor(constructor)
        {
        }

        bool operator<(const AbilityInfo& a, const AbilityInfo& b)
        {
            return a.Key < b.Key;
        }

        GameAbility::GameAbility(const std::string& key, AbilityConstructor<> constructor)
            : _key(key)
        {
            _abilityInfos[_key] = constructor;
        }

        const std::string& GameAbility::operator()() const
        {
            return _key;
        }

        AbilityConstructor<> GameAbility::GetConstructor(const std::string& key)
        {
            return _abilityInfos[key];
        }

        PreloadFunction GameAbility::GetPreloadFunction(const std::string& key)
        {
            return _abilityInfos[key].EnumeratePreloads;
        }

        std::vector<std::string> GameAbility::GetAllAbilities()
        {
            std::vector<std::string> keys;
            for (auto info : _abilityInfos)
            {
                keys.push_back(info.first);
            }
            return keys;
        }

        GameAbility::gameAbilityInfoMap GameAbility::_abilityInfos;

        template <typename AbilityType, typename... argsT>
        static GameAbility BindGameAbility(const std::string& key, argsT... args)
        {
            using namespace std::placeholders;
            return GameAbility(key, BindAbilityConstructor<AbilityType, argsT...>(std::forward<argsT>(args)...));
        }

        // Abilities

        // Brewer
        const GameAbility Abilities::ThrowBomb = BindGameAbility<Dwarf::Ability::ThrowBomb>("ThrowBomb");
        const GameAbility Abilities::PlantDynamite = BindGameAbility<Dwarf::Ability::PlantDynamite>("PlantDynamite");
        const GameAbility Abilities::BrewBeer = BindGameAbility<Dwarf::Ability::BrewBeer>("BrewBeer");
        const GameAbility Abilities::BrewWine = BindGameAbility<Dwarf::Ability::BrewWine>("BrewWine");

        // Builder
        const GameAbility Abilities::BuildBarricade = BindGameAbility<Dwarf::Ability::BuildBarricade>("BuildBarricade");
        const GameAbility Abilities::BuildBridge = BindGameAbility<Dwarf::Ability::BuildBridge>("BuildBridge");

        // Cook
        const GameAbility Abilities::CookBeef = BindGameAbility<Dwarf::Ability::CookBeef>("CookBeef");
        const GameAbility Abilities::CookChicken = BindGameAbility<Dwarf::Ability::CookChicken>("CookChicken");
        const GameAbility Abilities::CookHam = BindGameAbility<Dwarf::Ability::CookHam>("CookHam");
        const GameAbility Abilities::CookAttackSpeedFood = BindGameAbility<Dwarf::Ability::CookAttackSpeedFood>("CookAttackSpeedFood");
        const GameAbility Abilities::CookCritFood = BindGameAbility<Dwarf::Ability::CookCritFood>("CookCritFood");
        const GameAbility Abilities::ChannelingFoodHeal = BindGameAbility<Dwarf::Ability::ChannelingFoodHeal>("ChannelingFoodHeal");

        // Fighter
        const GameAbility Abilities::Leap = BindGameAbility<Dwarf::Ability::Leap>("Leap");

        // Miner
        const GameAbility Abilities::Sprint = BindGameAbility<Dwarf::Ability::Sprint>("Sprint");
        const GameAbility Abilities::SackSmash = BindGameAbility<Dwarf::Ability::SackSmash>("SackSmash");

        // Navigator
        const GameAbility Abilities::Grapple = BindGameAbility<Dwarf::Ability::Grapple>("Grapple");
        const GameAbility Abilities::ShootFlare = BindGameAbility<Dwarf::Ability::ShootFlare>("ShootFlare");
    }

    void EnumerateFindableAbilitySetPreloads(PreloadSet& preloads, const Ability::FindableAbilitySet& abilities)
    {
        for (const auto& classType : abilities)
        {
            for (const auto& abilityType : classType.second)
            {
                Ability::GameAbility::GetPreloadFunction(abilityType)(preloads);
            }
        }
    }
}
