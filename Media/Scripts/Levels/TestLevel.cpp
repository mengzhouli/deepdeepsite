#include "Levels/TestLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        TestLevel::TestLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : CampaignLevel(parameters, campaignParameters)
        {
        }

        TestLevel::~TestLevel()
        {
        }

        CampaignLevelInfo TestLevel::GetCampaignLevelInfo()
        {
            CampaignLevelInfo info;
            info.MaxTotalCharacters = 10;
            info.StartingResources = Item::Resources(150);

            return info;
        }
    }

    template <>
    void EnumeratePreloads<Level::TestLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::CampaignLevel>(preloads);
    }
}
