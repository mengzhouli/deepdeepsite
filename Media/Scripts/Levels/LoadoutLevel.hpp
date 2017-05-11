#pragma once

#include "Levels/BasicLevel.hpp"
#include "Levels/CampaignLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        class LoadoutLevel : public BasicLevel
        {
        public:
            LoadoutLevel(const LevelParameters& parameters, const CampaignLevelInfo& campaignLevelInfo, LevelChangeFunction levelChangeFunc,
                         PreloadLevelFunction preloadLevelFunc, ExitGameFunction exitFunc, TransitionToCampaignMenuFunction menuFunc,
                         PauseGameFunction pauseFunc, SaveProfileFunction saveProfileFunc, CampaignLevelConstructor<> campaignLevel);

        protected:
            virtual ~LoadoutLevel();

            virtual void OnCreate() override;

        private:
            CampaignLevelInfo _campaignLevelInfo;
            LevelChangeFunction _levelChangeFunc;
            PreloadLevelFunction _preloadLevelFunc;
            TransitionToCampaignMenuFunction _menuFunc;
            ExitGameFunction _exitFunc;
            PauseGameFunction _pauseFunc;
            SaveProfileFunction _saveProfileFunc;
            CampaignLevelConstructor<> _campaignLevel;
        };
    }

    template <>
    void EnumeratePreloads<Level::LoadoutLevel>(PreloadSet& preloads);
}
