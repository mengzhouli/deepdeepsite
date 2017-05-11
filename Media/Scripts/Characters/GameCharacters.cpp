#include "Characters/GameCharacters.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        GameCharacter::GameCharacter(const std::string& key, CharacterConstructor<> constructor,  PreloadFunction preloadFunction)
            : _key(key)
        {
            _characterInfos[_key].constructor = constructor;
            _characterInfos[_key].preloadFunction = preloadFunction;
        }

        const std::string& GameCharacter::operator()() const
        {
            return _key;
        }

        CharacterConstructor<> GameCharacter::GetConstructor(const std::string& key)
        {
            return _characterInfos.at(key).constructor;
        }

        PreloadFunction GameCharacter::GetPreloadFunction(const std::string& key)
        {
            return _characterInfos.at(key).preloadFunction;
        }

        std::vector<std::string> GameCharacter::GetAllCharacters()
        {
            std::vector<std::string> keys;
            for (auto info : _characterInfos)
            {
                keys.push_back(info.first);
            }
            return keys;
        }

        GameCharacter::gameCharacterInfoMap GameCharacter::_characterInfos;

        template <typename CharacterType, typename... argsT>
        static GameCharacter BindGameCharacter(const std::string& key, argsT... args)
        {
            using namespace std::placeholders;
            return GameCharacter(key, BindCharacterConstructor<CharacterType, argsT...>(std::forward<argsT>(args)...), EnumeratePreloads<CharacterType>);
        }

        const GameCharacter Dwarves::Brewer    = BindGameCharacter<BrewerDwarf>("Brewer");
        const GameCharacter Dwarves::Builder   = BindGameCharacter<BuilderDwarf>("Builder");
        const GameCharacter Dwarves::Cook      = BindGameCharacter<CookDwarf>("Cook");
        const GameCharacter Dwarves::Fighter   = BindGameCharacter<FighterDwarf>("Fighter");
        const GameCharacter Dwarves::Miner     = BindGameCharacter<MinerDwarf>("Miner");
        const GameCharacter Dwarves::Navigator = BindGameCharacter<NavigatorDwarf>("Navigator");
    }
}
