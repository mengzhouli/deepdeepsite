#pragma once

#include "Levels/CampaignLevel.hpp"

#include "Characters/DestructableRocks.hpp"
#include "Characters/Chest.hpp"
#include "Characters/Door.hpp"

namespace Dwarf
{
    namespace Level
    {
        class DwarfHomeCheckpoint : public CampaignLevelCheckpoint
        {
        public:
            DwarfHomeCheckpoint();
            virtual ~DwarfHomeCheckpoint();

            bool GetDoorState(LayerID layer, TriggerID trigger) const;
            void SetDoorState(LayerID layer, TriggerID trigger, bool state);

            Item::Resources GetOreState(LayerID layer, TriggerID trigger) const;
            void SetOreState(LayerID layer, TriggerID trigger, const Item::Resources& state);

            bool WasChestOpened(LayerID layer, TriggerID trigger) const;
            void SetChestOpened(LayerID layer, TriggerID trigger, bool opened);

        private:
            std::unordered_map<LayerID, std::unordered_map<TriggerID, bool>> _doorStates;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, Item::Resources>> _oreStates;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, bool>> _chestStates;
        };

        class DwarfHomeLevel : public CampaignLevel
        {
        public:
            DwarfHomeLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

        protected:
            virtual ~DwarfHomeLevel();

            std::vector<Character::CharacterID> SpawnLadders(LevelLayerInstance* layer, const std::string& splineName);
            std::vector<Character::CharacterID> SpawnGrapples(LevelLayerInstance* layer, const std::string& splineName);

            Character::CharacterID SpawnTrackedDoor(LevelLayerInstance* layer, const std::string& leverSpawnName, const std::string& doorSpawnName,
                                                    const std::string& collisionClosedName, const std::string& collisionOpenName, Character::DoorSize size, bool startOpen, bool inverted,
                                                    Character::OpenStateChangeCallback stateChangeCallback,std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);

            Character::CharacterID SpawnTrackedRubble(LevelLayerInstance* layer, Character::RubbleSize size, const std::string& triggerName, const std::string& collisionName,
                                                      Character::OnDestructionCallback callback, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);
            
            Character::CharacterID SpawnTrackedDestructableWall(LevelLayerInstance* layer, const std::string& triggerName, const std::string& collisionName,
                                                                Character::OnDestructionCallback callback, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);

            std::vector<Character::CharacterID> SpawnTrackedOres(LevelLayerInstance* layer, const std::string& triggerName, const Item::Resources& resources,
                                                                 std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);

            Character::CharacterID SpawnTrackedChest(LevelLayerInstance* layer, const std::string& spawnTrigger, bool invertX,
                                                     const Item::FindableItemSet& items, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);

            virtual void OnCreate() override;
            virtual Splinef GetCharacterSpawnArea() const override;

            virtual void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint) override;
            virtual void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile) override;

        private:
            Character::CharacterID SpawnTrackedOre(LevelLayerInstance* layer, const Vector2f& position, TriggerID id, const Item::Resources& resources,
                                                   std::shared_ptr<const DwarfHomeCheckpoint> checkpoint);

            std::unordered_map<LayerID, std::unordered_map<TriggerID, Character::CharacterID>> _spawnedDoors;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, Character::CharacterID>> _spawnedOres;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, bool>> _openedChests;

            Item::FindableItemSet _foundItems;
        };
    }

    template <>
    void EnumeratePreloads<Level::DwarfHomeLevel>(PreloadSet& preloads);
}
