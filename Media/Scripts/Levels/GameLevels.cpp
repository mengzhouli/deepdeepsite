#include "Levels/GameLevels.hpp"

#include "Levels/DwarfHomes/DwarfHome0.hpp"
#include "Levels/DwarfHomes/DwarfHome1.hpp"
#include "Levels/DwarfHomes/DwarfHome2.hpp"
#include "Levels/DwarfHomes/DwarfHome3.hpp"
#include "Levels/DwarfHomes/DwarfHome4.hpp"

namespace Dwarf
{
    namespace Level
    {
        GameCampaignLevel::GameCampaignLevel(const std::string& key, CampaignLevelConstructor<> constructor,
                              std::function<CampaignLevelInfo()> info, PreloadFunction preloadFunction)
            : _key(key)
        {
            _levelInfos[_key].constructor = constructor;
            _levelInfos[_key].info = info;
            _levelInfos[_key].preloadFunction = preloadFunction;
        }

        const std::string& GameCampaignLevel::operator()() const
        {
            return _key;
        }

        CampaignLevelConstructor<> GameCampaignLevel::GetConstructor(const std::string& key)
        {
            return _levelInfos[key].constructor;
        }

        CampaignLevelInfo GameCampaignLevel::GetInfo(const std::string& key)
        {
            return _levelInfos[key].info();
        }

        PreloadFunction GameCampaignLevel::GetPreloadFunction(const std::string& key)
        {
            return _levelInfos[key].preloadFunction;
        }

        std::vector<std::string> GameCampaignLevel::GetAllLevels()
        {
            std::vector<std::string> keys;
            for (auto info : _levelInfos)
            {
                keys.push_back(info.first);
            }
            return keys;
        }

        GameCampaignLevel::gameLevelInfoMap GameCampaignLevel::_levelInfos;

        template <typename LevelType, typename... argsT>
        GameCampaignLevel BindCampaignLevel(const std::string& key, const std::string& path, argsT... args)
        {
            using namespace std::placeholders;

            PreloadFunction preloadFunc = [=](PreloadSet& preloads)
            {
                preloads.insert(path);
                EnumeratePreloads<LevelType>(preloads);
            };

            return GameCampaignLevel(key, BindCampaignLevelConstructor<LevelType, argsT...>(path, std::forward<argsT>(args)...),
                                     std::bind(&LevelType::GetCampaignLevelInfo), preloadFunc);
        }

        // Levels

        // Dwarf Homes
        const GameCampaignLevel Campaign::DwarfHomes0 = BindCampaignLevel<Dwarf::Level::DwarfHome0>("DwarfHomes0", "Levels/dwarfhomes_0.lvl");
        const GameCampaignLevel Campaign::DwarfHomes1 = BindCampaignLevel<Dwarf::Level::DwarfHome1>("DwarfHomes1", "Levels/dwarfhomes_1.lvl");
        const GameCampaignLevel Campaign::DwarfHomes2 = BindCampaignLevel<Dwarf::Level::DwarfHome2>("DwarfHomes2", "Levels/dwarfhomes_2.lvl");
        const GameCampaignLevel Campaign::DwarfHomes3 = BindCampaignLevel<Dwarf::Level::DwarfHome3>("DwarfHomes3", "Levels/dwarfhomes_3.lvl");
        const GameCampaignLevel Campaign::DwarfHomes4 = BindCampaignLevel<Dwarf::Level::DwarfHome4>("DwarfHomes4", "Levels/dwarfhomes_4.lvl");
    }
}
