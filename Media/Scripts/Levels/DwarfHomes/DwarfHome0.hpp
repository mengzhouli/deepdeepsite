#pragma once

#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        class DwarfHome0Checkpoint : public DwarfHomeCheckpoint
        {
        public:
            DwarfHome0Checkpoint();
            virtual ~DwarfHome0Checkpoint();

            bool WasFlareCutscenePlayed() const;
            void SetFlareCutscenePlayed(bool played);

        private:
            bool _flareCutscenePlayed;
        };

        class DwarfHome0 : public DwarfHomeLevel
        {
        public:
            DwarfHome0(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

        protected:
            virtual ~DwarfHome0();

            virtual void OnCreate() override;
            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnSpawnCampaignCharacter(Character::Character* character) override;

            virtual void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

            virtual void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;

        private:
            bool onPortraitUsed(Character::Character* character);

            bool _playedFlareCutscene;
            Character::CharacterID _flarePickupID;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome0>(PreloadSet& preloads);
}
