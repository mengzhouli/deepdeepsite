#pragma once

#include "Ability/Ability.hpp"
#include "Content/Preload.hpp"

#include <string>
#include <map>
#include <functional>

namespace Dwarf
{
    namespace Ability
    {
        using FindableAbilitySet = std::map<std::string, std::set<std::string>>;

        struct AbilityInfo
        {
            std::string Key;
            AbilityConstructor<> Constructor;

            AbilityInfo();
            AbilityInfo(std::string key, AbilityConstructor<> constructor);
        };

        bool operator<(const AbilityInfo& a, const AbilityInfo& b);

        class GameAbility
        {
        public:
            GameAbility(const std::string& key, AbilityConstructor<> constructor);

            const std::string& operator()() const;

            static AbilityConstructor<> GetConstructor(const std::string& key);
            static PreloadFunction GetPreloadFunction(const std::string& key);

            static std::vector<std::string> GetAllAbilities();

        private:
            std::string _key;

            typedef std::map<std::string, AbilityConstructor<>> gameAbilityInfoMap;
            static gameAbilityInfoMap _abilityInfos;
        };

        struct Abilities
        {
            // Brewer
            static const GameAbility ThrowBomb;
            static const GameAbility PlantDynamite;
            static const GameAbility BrewBeer;
            static const GameAbility BrewWine;

            // Builder
            static const GameAbility BuildBarricade;
            static const GameAbility BuildBridge;

            // Cook
            static const GameAbility CookBeef;
            static const GameAbility CookChicken;
            static const GameAbility CookHam;
            static const GameAbility CookCritFood;
            static const GameAbility CookAttackSpeedFood;
            static const GameAbility ChannelingFoodHeal;

            // Fighter
            static const GameAbility Leap;

            // Miner
            static const GameAbility Sprint;
            static const GameAbility SackSmash;

            // Navigator
            static const GameAbility Grapple;
            static const GameAbility ShootFlare;
        };
    }

    void EnumerateFindableAbilitySetPreloads(PreloadSet& preloads, const Ability::FindableAbilitySet& abilties);
}
