#pragma once

#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"

#include "Drawables/Flame.hpp"
#include "Lights/PolygonLight.hpp"

namespace Dwarf
{
    namespace Audio
    {
        class LavaSound;
    };

    namespace Level
    {
        class DwarfHome3Checkpoint : public DwarfHomeCheckpoint
        {
        public:
            DwarfHome3Checkpoint();
            virtual ~DwarfHome3Checkpoint();

            bool WasDynamiteCutscenePlayed() const;
            void SetDynamiteCutscenePlayed(bool played);

            bool WasBossAreaLightTurnedOn() const;
            void SetBossAreaLightTurnedOn(bool turnedOn);

        private:
            bool _dynamiteCutscenePlayed;
            bool _bossAreaLightTurnedOn;
        };

        class DwarfHome3 : public DwarfHomeLevel
        {
        public:
            DwarfHome3(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

        protected:
            virtual ~DwarfHome3();

            virtual void OnCreate() override;
            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnSpawnCampaignCharacter(Character::Character* character) override;

            virtual void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

            virtual void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Character::CharacterID _fighterPortraitID;

            bool _playedDynamiteCutscene = false;
            bool _playedBossCutscene = false;

            bool _bossAreaLightOn;
            float _bossAreaLightOnTime;
            Lights::PolygonLight _bossAreaLight;

            Character::CharacterID _boss = 0;

            std::vector<std::unique_ptr<Graphics::Flame>> _lavaFlames;
            std::unique_ptr<Audio::LavaSound> _lavaSound;

            bool _builderPortraitUsed = false;
            bool _minerPortraitUsed = false;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome3>(PreloadSet& preloads);
}
