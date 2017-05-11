#include "Characters/Checkpoint.hpp"

namespace Dwarf
{
    namespace Level
    {
        template <typename T>
        std::vector<T*> CampaignLevel::SpawnTrackedCharactersAtTriggers(const std::string& triggerLocations, Character::CharacterConstructor<T> constructor,
                                                                        const std::string& nameBase, Character::Controller* controller,
                                                                        std::shared_ptr<const CampaignLevelCheckpoint> checkpoint)
        {
            std::vector<T*> result;

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                LevelLayerInstance* layer = GetLayer(i);
                std::vector<TriggerID> ids;
                std::vector<Vector2f> spawns = layer->GetTriggerPositions(triggerLocations, &ids);
                for (uint32_t j = 0; j < spawns.size(); j++)
                {
                    if (checkpoint == nullptr || !checkpoint->WasCharacterKilled(layer->GetID(), ids[j]))
                    {
                        T* character = layer->SpawnCharacter(spawns[j], Format("%s_%u", nameBase.c_str(), j), controller, constructor);
                        _spawnedCharacters[layer->GetID()][ids[j]] = character->GetID();
                        result.push_back(character);
                    }
                }
            }
            return result;
        }

        template <typename CheckpointType>
        void CampaignLevel::SpawnCheckpoint(const Splinef& position, const std::string& name, bool captureOnFirstUpdate)
        {
            uint32_t checkpointIdx = _checkpointStates.size();
            std::shared_ptr<CampaignLevelCheckpoint> newCheckpoint = nullptr;

            std::shared_ptr<const CampaignLevelCheckpoint> checkpoint = GetCheckpoint();
            if (checkpoint)
            {
                const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& checkpointCheckpoints = checkpoint->GetCheckpointStates();
                assert(checkpointIdx < checkpointCheckpoints.size());
                newCheckpoint = checkpointCheckpoints[checkpointIdx];
            }

            assert(newCheckpoint == nullptr || newCheckpoint->GetNameCode() == name);
            _checkpointStates.push_back(newCheckpoint);

            Character::OnCheckpointUseCallback checkpointUseCallback = [=](Character::Character* character)
            {
                std::shared_ptr<CampaignLevelCheckpoint> state = std::make_shared<CheckpointType>();
                state->SetCharacterSpawnArea(position);
                state->SetNameCode(name);
                PopulateCheckpoint(state);
                _checkpointStates[checkpointIdx] = state;
            };

            bool alreadyCaptured = newCheckpoint != nullptr;

            Character::CharacterConstructor<> constructor =
                Character::BindCharacterConstructor<Character::Checkpoint>(BaseFireColor, checkpointUseCallback, alreadyCaptured, captureOnFirstUpdate);
            GetPrimaryLayer()->SpawnCharacter(position.Evalulate(0.5f), name, nullptr, constructor);
        }

        template <typename T>
        std::shared_ptr<const T> CampaignLevel::GetCheckpoint() const
        {
            return AsA<T>(_campaignParameters.Checkpoint);
        }
    }
}
