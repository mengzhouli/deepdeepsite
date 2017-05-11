#pragma once

#include "Levels/BasicLevel.hpp"
#include "GameState/LevelGameState.hpp"
#include "Character/Controller.hpp"
#include "Items/GameItems.hpp"
#include "Abilities/GameAbilities.hpp"
#include "Characters/GameCharacters.hpp"

#include <typeindex>
#include <map>

namespace Dwarf
{
    namespace Settings
    {
        class TheDeepDeepProfile;
    }

    namespace Character
    {
        class Player;
        class FlameHolder;
        class Bridge2;
    }

    namespace Level
    {
        struct CampaignLevelInfo
        {
            std::string NameCode;

            uint32_t MaxTotalCharacters;
            std::map<std::type_index, uint32_t> MinTypedCharacters;

            Item::Resources StartingResources;

            Character::FindableCharacterSet FindableCharacters;
            Item::FindableItemSet FindableItems;
            Ability::FindableAbilitySet FindableAbilities;
        };

        using UnlockFraction = std::pair<uint32_t, uint32_t>;

        void UnlockItems(Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items);
        Item::FindableItemSet ComputeRemainingFindableItems(const Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items);
        uint32_t ComputeItemCount(const Item::FindableItemSet& items);
        UnlockFraction ComputeUnlockedItemFraction(const Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items);

        void UnlockCharacters(Settings::TheDeepDeepProfile* profile, const Character::FindableCharacterSet& characters);
        UnlockFraction ComputeUnlockedCharacterFraction(const Settings::TheDeepDeepProfile* profile, const Character::FindableCharacterSet& characters);

        void UnlockAbilities(Settings::TheDeepDeepProfile* profile, const Ability::FindableAbilitySet& abilities);
        UnlockFraction ComputeUnlockedAbilityFraction(const Settings::TheDeepDeepProfile* profile, const Ability::FindableAbilitySet& abilities);

        struct CampaignLevelCharacterInfo
        {
            Character::CharacterConstructor<> Constructor;
            std::vector<Item::ItemConstructor<>> Items;
            std::vector<Ability::AbilityConstructor<>> Abilities;
        };

        class CampaignLevelCheckpoint
        {
        public:
            CampaignLevelCheckpoint();
            virtual ~CampaignLevelCheckpoint();

            const Splinef& GetCharacterSpawnArea() const;
            void SetCharacterSpawnArea(const Splinef& spawnArea);

            const std::string& GetNameCode() const;
            void SetNameCode(const std::string& name);

            bool WasCharacterKilled(LayerID layer, TriggerID spawnTrigger) const;
            void SetCharacterKilled(LayerID layer, TriggerID spawnTrigger);

            bool ShouldFlameHolderBeOn(LayerID layer, TriggerID spawnTrigger) const;
            void SetFlameHolderOn(LayerID layer, TriggerID spawnTrigger, bool on);

            bool WasPlayerCharacterKilled(uint32_t idx) const;
            void SetPlayerCharacterKilled(uint32_t idx);

            bool WasCheckpointReached(uint32_t checkpoint) const;
            void SetCheckpointReached(uint32_t checkpoint);

            const std::vector<std::pair<Vector2f, Vector2f>>& GetGrapplePositions(LayerID layer) const;
            void AddGrapplePosition(LayerID layer, const Vector2f& a, const Vector2f& b);

            float GetBridgeBuildPercentage(LayerID layer, SplineID spline) const;
            void SetBridgeBuildPercentage(LayerID layer, SplineID spline, float buildperc);

            const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& GetCheckpointStates() const;
            void SetCheckpointStates(const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& checkpoints);

            bool WasTriggeredSwellPlayed(const std::string& name) const;
            void SetTriggeredSwellPlayed(const std::string& name);

        private:
            Splinef _spawnArea;
            std::string _nameCode;
            std::unordered_map<LayerID, std::unordered_set<TriggerID>> _killedCharacters;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, bool>> _flameHolderStates;
            std::unordered_set<uint32_t> _killedPlayerCharacters;
            std::unordered_set<uint32_t> _reachedCheckpoints;
            std::unordered_map<LayerID, std::vector<std::pair<Vector2f, Vector2f>>> _grapples;
            std::unordered_map<LayerID, std::unordered_map<SplineID, float>> _bridges;
            std::vector<std::shared_ptr<CampaignLevelCheckpoint>> _checkpoints;
            std::unordered_set<std::string> _playedTriggeredSwells;
        };
    }

    typedef std::function<void(std::shared_ptr<const Level::CampaignLevelCheckpoint>)> RestartAtCheckpointFunction;
    typedef std::function<void()> RestartAtLoadoutFunction;

    namespace Level
    {
        struct CampaignLevelParameters
        {
            std::vector<CampaignLevelCharacterInfo> Characters;
            Item::Resources StartingResources;

            std::shared_ptr<const CampaignLevelCheckpoint> Checkpoint;

            ExitGameFunction ExitFunction;
            TransitionToCampaignMenuFunction MenuFunction;
            RestartAtCheckpointFunction CheckpointFunction;
            RestartAtLoadoutFunction LoadoutFunction;
            PauseGameFunction PauseFunction;
            SaveProfileFunction SaveProfile;
        };

        class CampaignLevel;

        template <typename T = CampaignLevel>
        using CampaignLevelConstructor = std::function<GameState::LevelConstructor<T>(const CampaignLevelParameters&)>;

        template <typename T, typename... argsT>
        CampaignLevelConstructor<T> BindCampaignLevelConstructor(const std::string& path, argsT... args)
        {
            using namespace std::placeholders;
            return std::bind(GameState::BindLevelConstructor<T, const CampaignLevelParameters&, argsT...>, path, _1, std::forward<argsT>(args)...);
        }

        class CampaignLevel : public BasicLevel
        {
        public:
            CampaignLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters);

            void RestartAtCheckpoint(uint32_t checkpoint);
            void RestartAtLoadout();

            void TransitionToCampaignMenu();

            const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& GetCheckpoints() const;

        protected:
            virtual ~CampaignLevel();

            void AddMusicTracks(const Audio::MusicTrackMap& tracks);
            void AddTrackedTriggeredMusicSwell(const std::string& triggerName, const std::string& swellName, std::shared_ptr<const CampaignLevelCheckpoint> checkpoint);

            void OnCreate() override;

            void OnUpdate(double totalTime, float dt) override;

            virtual Splinef GetCharacterSpawnArea() const;
            virtual void OnSpawnCampaignCharacter(Character::Character* character);

            void CompleteLevel(bool success);
            virtual void OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile);

            Character::Controller* GetPlayerController() const;
            Character::Controller* GetMonsterController() const;
            Character::Controller* GetWildlifeController() const;

            template <typename T>
            std::vector<T*> SpawnTrackedCharactersAtTriggers(const std::string& triggerLocations, Character::CharacterConstructor<T> constructor,
                                                             const std::string& nameBase, Character::Controller* controller,
                                                             std::shared_ptr<const CampaignLevelCheckpoint> checkpoint);

            std::vector<Character::FlameHolder*> SpawnTrackedFlameHoldersAtTriggers(const std::string& triggerLocations,
                                                                                    Character::CharacterConstructor<Character::FlameHolder> constructor,
                                                                                    const std::string& nameBase,
                                                                                    std::shared_ptr<const CampaignLevelCheckpoint> checkpoint);

            std::vector<Character::Bridge2*> SpawnTrackedBridgesAtSplines(const std::string& splineLocations, float buildPerc, const Item::Resources& buildCost,
                                                                          std::shared_ptr<const CampaignLevelCheckpoint> checkpoint);

            template <typename CheckpointType>
            void SpawnCheckpoint(const Splinef& position, const std::string& name, bool captureOnFirstUpdate);

            template <typename T = CampaignLevelCheckpoint>
            std::shared_ptr<const T> GetCheckpoint() const;

            virtual void PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint);

        private:
            CampaignLevelParameters _campaignParameters;

            Character::Player* _playerController;
            Character::Controller* _monsterController;
            Character::Controller* _wildlifeController;

            std::unordered_map<LayerID, std::unordered_map<TriggerID, Character::CharacterID>> _spawnedCharacters;
            std::unordered_map<LayerID, std::unordered_map<TriggerID, Character::CharacterID>> _spawnedFlameHolders;
            std::unordered_map<LayerID, std::unordered_map<SplineID, Character::CharacterID>> _spawnedBridges;
            std::unordered_map<uint32_t, Character::CharacterID> _playerCharacters;

            std::vector<std::shared_ptr<CampaignLevelCheckpoint>> _checkpointStates;

            std::vector<std::string> _musicTrackAreas;

            std::map<std::string, std::vector<std::string>> _triggeredMusicSwells;
            std::unordered_set<std::string> _playedTriggeredMusicSwells;

            std::unordered_map<LayerID, std::list<Polygonf>> _aggroZones;
        };
    }

    template <>
    void EnumeratePreloads<Level::CampaignLevel>(PreloadSet& preloads);
}

#include "CampaignLevel.inl"
#include "Settings/TheDeepDeepProfile.hpp"
