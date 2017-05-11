#pragma once

#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"
#include "RainEffect.hpp"

namespace Dwarf
{
    namespace Level
    {
        class DwarfHome2Checkpoint : public DwarfHomeCheckpoint
        {
        public:
            DwarfHome2Checkpoint();
            virtual ~DwarfHome2Checkpoint();

            bool DidPlayEatingCutscene() const;
            void SetPlayedEatingCutscene(bool played);

            bool WasBossKilled() const;
            void SetBossKilled(bool killed);

            bool WasLeapPickedUp() const;
            void SetLeapPickedUp(bool pickedUp);

        private:
            bool _playedEatingCutscene;
            bool _bossKilled;
            bool _leapPickedUp;
        };

        class DwarfHome2 : public DwarfHomeLevel
        {
        public:
            DwarfHome2(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

            void InitializeDebugger(HUD::Debugger* debugger) override;

        protected:
            ~DwarfHome2() override;

            void OnCreate() override;
            void OnDestroy() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

            void OnSpawnCampaignCharacter(Character::Character* character) override;

            void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

            void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;

        private:
            bool _playedEatingCutscene;

            bool _playedBossCutscene;
            Character::CharacterID _bossLadderID;
            Character::CharacterID _bossID;
            Character::CharacterID _bossBrazierID;

            bool _playedLeapCutscene;
            Character::CharacterID _leapPickupID;

            std::unique_ptr<RainEffect> _rain = nullptr;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome2>(PreloadSet& preloads);
}
