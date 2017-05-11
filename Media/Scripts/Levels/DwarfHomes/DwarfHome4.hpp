#pragma once

#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"

#include "Lights/PolygonLight.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class BasketRopeDrawable;
        class Flame;
    }

    namespace Audio
    {
        class LavaSound;
    };

    namespace Level
    {
        class DwarfHome4Checkpoint : public DwarfHomeCheckpoint
        {
        public:
            DwarfHome4Checkpoint();
            virtual ~DwarfHome4Checkpoint();

            bool WasForgeDoorOpened() const;
            void SetForgeDoorOpened(bool opened);

            bool WasBossKilled() const;
            void SetBossKilled(bool killed);

        private:
            bool _forgeDoorOpened = false;
            bool _bossKilled = false;
        };

        class DwarfHome4 : public DwarfHomeLevel
        {
        public:
            DwarfHome4(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

        protected:
            virtual ~DwarfHome4();

            void OnCreate() override;
            void OnDestroy() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const override;

            void OnSpawnCampaignCharacter(Character::Character* character) override;

            void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

            void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;

        private:
            void forgeGobboAttack();

            bool _forgeAreaLightOn;
            float _forgeAreaLightOnTime;
            Lights::PolygonLight _forgeAreaLight;
            Lights::PolygonLight _mainAreaLight;

            std::vector<std::unique_ptr<Graphics::BasketRopeDrawable>> _basketRopes;

            std::unique_ptr<Audio::LavaSound> _lavaSound;

            float _flameRadius;
            std::vector<std::unique_ptr<Graphics::Flame>> _mainFlames;
            std::vector<std::unique_ptr<Graphics::Flame>> _forgeFlames;
            std::vector<Lights::LineLight> _forgePillarLights;
            std::vector<Lights::PointLight> _forgeThroneLights;

            Character::CharacterID _bossID = 0;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome4>(PreloadSet& preloads);
}
