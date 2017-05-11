#pragma once

#include "Levels/CampaignLevel.hpp"

#include <string>
#include <map>
#include <functional>

namespace Dwarf
{
    namespace Level
    {
        class GameCampaignLevel
        {
        public:
            GameCampaignLevel(const std::string& key, CampaignLevelConstructor<> constructor,
                              std::function<CampaignLevelInfo()> info, PreloadFunction preloadFunction);

            const std::string& operator()() const;

            static CampaignLevelConstructor<> GetConstructor(const std::string& key);
            static CampaignLevelInfo GetInfo(const std::string& key);
            static PreloadFunction GetPreloadFunction(const std::string& key);

            static std::vector<std::string> GetAllLevels();

        private:
            std::string _key;

            struct LevelInfo
            {
                CampaignLevelConstructor<> constructor;
                std::function<CampaignLevelInfo()> info;
                PreloadFunction preloadFunction;
            };

            typedef std::map<std::string, LevelInfo> gameLevelInfoMap;
            static gameLevelInfoMap _levelInfos;
        };

        struct Campaign
        {
            // Dwarf Homes
            static const GameCampaignLevel DwarfHomes0;
            static const GameCampaignLevel DwarfHomes1;
            static const GameCampaignLevel DwarfHomes2;
            static const GameCampaignLevel DwarfHomes3;
            static const GameCampaignLevel DwarfHomes4;
        };

        struct Challenge
        {

        };
    }
}
