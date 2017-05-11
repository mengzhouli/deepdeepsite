#pragma once

#include "Levels/CampaignLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        class TestLevel : public CampaignLevel
        {
        public:
            TestLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

        protected:
            virtual ~TestLevel();
        };
    }

    template <>
    void EnumeratePreloads<Level::TestLevel>(PreloadSet& preloads);
}
