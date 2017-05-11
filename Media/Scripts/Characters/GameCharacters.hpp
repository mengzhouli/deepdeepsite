#pragma once

#include "Character/Character.hpp"
#include "Content/Preload.hpp"

#include <string>
#include <map>
#include <functional>
#include <typeindex>

namespace Dwarf
{
    namespace Character
    {
        using FindableCharacterSet = std::set<std::string>;

        class GameCharacter
        {
        public:
            GameCharacter(const std::string& key, CharacterConstructor<> constructor, PreloadFunction preloadFunction);

            const std::string& operator()() const;

            static CharacterConstructor<> GetConstructor(const std::string& key);
            static PreloadFunction GetPreloadFunction(const std::string& key);

            static std::vector<std::string> GetAllCharacters();

        private:
            std::string _key;

            struct CharacterInfo
            {
                CharacterConstructor<> constructor;
                PreloadFunction preloadFunction;
            };

            typedef std::map<std::string, CharacterInfo> gameCharacterInfoMap;
            static gameCharacterInfoMap _characterInfos;
        };

        struct Dwarves
        {
            static const GameCharacter Brewer;
            static const GameCharacter Builder;
            static const GameCharacter Cook;
            static const GameCharacter Fighter;
            static const GameCharacter Miner;
            static const GameCharacter Navigator;
        };
    }
}
