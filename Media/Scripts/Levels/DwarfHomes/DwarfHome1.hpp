#pragma once

#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"

namespace Dwarf
{
    namespace Level
    {
        class DwarfHome1Checkpoint : public DwarfHomeCheckpoint
        {
        public:
            DwarfHome1Checkpoint();
            virtual ~DwarfHome1Checkpoint();

            bool WasGrapplePickedUp() const;
            void SetGrapplePickedUp(bool pickedUp);

        private:
            bool _grapplePickedUp;
        };

        class DwarfHome1 : public DwarfHomeLevel
        {
        public:
            DwarfHome1(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            static CampaignLevelInfo GetCampaignLevelInfo();

        protected:
            virtual void OnCreate() override;
            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnSpawnCampaignCharacter(Character::Character* character) override;

            virtual void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

            void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;

        protected:
            virtual ~DwarfHome1();

        private:
            bool onPortraitUsed(Character::Character* character);
            bool onGrappleUsed(Character::Character* character);

            Character::CharacterID _cookPortraitID;

            bool _grapplePickedUp;
            Character::CharacterID _grapplePickupID;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHome1>(PreloadSet& preloads);
}
