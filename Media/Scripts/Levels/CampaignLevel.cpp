#include "Levels/CampaignLevel.hpp"
#include "Item/Item.hpp"

#include "Characters/Checkpoint.hpp"
#include "Characters/Torch.hpp"
#include "Characters/Bridge.hpp"
#include "Characters/GrappleRope.hpp"

#include "Controllers/Player.hpp"
#include "Controllers/MonsterController.hpp"
#include "Controllers/WildlifeController.hpp"

namespace Dwarf
{
    namespace Level
    {
        template<typename SetterFunc>
        static void AddItemsToProfile(Settings::TheDeepDeepProfile* profile, const Item::FindableItems& items, SetterFunc setter)
        {
            for (const auto& classItems : items)
            {
                for (const auto& item : classItems.second)
                {
                    uint32_t itemCount = item.second;
                    setter(profile, classItems.first, item.first, itemCount);
                }
            }
        }

        template<typename GetterFunc>
        static Item::FindableItems ComputeFindableItemsForProfile(const Settings::TheDeepDeepProfile* profile, const Item::FindableItems& items, GetterFunc getter)
        {
            Item::FindableItems result;
            for (const auto& classItems : items)
            {
                for (const auto& item : classItems.second)
                {
                    uint32_t curItemCount = getter(profile, classItems.first, item.first);
                    uint32_t newItemCount = item.second;
                    if (curItemCount < newItemCount)
                    {
                        if (newItemCount == Item::InfiniteItemCount)
                        {
                            result[classItems.first][item.first] = Item::InfiniteItemCount;
                        }
                        else
                        {
                            result[classItems.first][item.first] = newItemCount - curItemCount;
                        }
                    }
                }
            }
            return result;
        }

        static uint32_t AddItemCounts(uint32_t a, uint32_t b)
        {
            if (a == Item::InfiniteItemCount || b == Item::InfiniteItemCount)
            {
                return Item::InfiniteItemCount;
            }
            else
            {
                return a + b;
            }
        }

        static uint32_t GetItemCount(const Item::FindableItems& items)
        {
            uint32_t result = 0;
            for (const auto& classItems : items)
            {
                for (const auto& item : classItems.second)
                {
                    uint32_t itemCount = item.second;
                    result = AddItemCounts(result, itemCount);
                }
            }
            return result;
        }

        void UnlockItems(Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items)
        {
            using namespace std::placeholders;
            AddItemsToProfile(profile, items.Weapons, std::bind(&Settings::TheDeepDeepProfile::AddWeapon, _1, _2, _3, _4));
            AddItemsToProfile(profile, items.Armors, std::bind(&Settings::TheDeepDeepProfile::AddArmor, _1, _2, _3, _4));
            AddItemsToProfile(profile, items.Trinkets, std::bind(&Settings::TheDeepDeepProfile::AddTrinket, _1, _2, _3, _4));
        }

        Item::FindableItemSet ComputeRemainingFindableItems(const Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items)
        {
            using namespace std::placeholders;
            Item::FindableItemSet result;
            result.Weapons = ComputeFindableItemsForProfile(profile, items.Weapons, std::bind(&Settings::TheDeepDeepProfile::GetWeaponTypeCount, _1, _2, _3));
            result.Armors = ComputeFindableItemsForProfile(profile, items.Armors, std::bind(&Settings::TheDeepDeepProfile::GetArmorTypeCount, _1, _2, _3));
            result.Trinkets = ComputeFindableItemsForProfile(profile, items.Trinkets, std::bind(&Settings::TheDeepDeepProfile::GetTrinketTypeCount, _1, _2, _3));
            return result;
        }

        uint32_t ComputeItemCount(const Item::FindableItemSet& items)
        {
            uint32_t result = 0;
            result = AddItemCounts(result, GetItemCount(items.Weapons));
            result = AddItemCounts(result, GetItemCount(items.Armors));
            result = AddItemCounts(result, GetItemCount(items.Trinkets));
            return result;
        }

        template<typename GetterFunc>
        static Vector2u GetUnlockItemFraction(const Settings::TheDeepDeepProfile* profile, const Item::FindableItems& items, GetterFunc getter)
        {
            Vector2u result = Vector2u::Zero;

            for (const auto& classItems : items)
            {
                for (const auto& item : classItems.second)
                {
                    uint32_t maxItemCount = item.second;
                    uint32_t curItemCount = Min(getter(profile, classItems.first, item.first), maxItemCount);

                    result.X += curItemCount != Item::InfiniteItemCount ? curItemCount : 1;
                    result.Y += maxItemCount != Item::InfiniteItemCount ? maxItemCount : 1;
                }
            }

            return result;
        }

        UnlockFraction ComputeUnlockedItemFraction(const Settings::TheDeepDeepProfile* profile, const Item::FindableItemSet& items)
        {
            using namespace std::placeholders;
            Vector2u fraction =
                GetUnlockItemFraction(profile, items.Weapons, std::bind(&Settings::TheDeepDeepProfile::GetWeaponTypeCount, _1, _2, _3)) +
                GetUnlockItemFraction(profile, items.Armors, std::bind(&Settings::TheDeepDeepProfile::GetArmorTypeCount, _1, _2, _3)) +
                GetUnlockItemFraction(profile, items.Trinkets, std::bind(&Settings::TheDeepDeepProfile::GetTrinketTypeCount, _1, _2, _3));
            return UnlockFraction(fraction.X, fraction.Y);
        }

        void UnlockCharacters(Settings::TheDeepDeepProfile* profile, const Character::FindableCharacterSet& characters)
        {
            for (const auto& characterType : characters)
            {
                profile->UnlockCharacterType(characterType);
            }
        }

        UnlockFraction ComputeUnlockedCharacterFraction(const Settings::TheDeepDeepProfile* profile, const Character::FindableCharacterSet& characters)
        {
            UnlockFraction result(0, 0);

            for (const auto& characterType : characters)
            {
                result.second++;
                if (profile->IsCharacterTypeUnlocked(characterType))
                {
                    result.first++;
                }
            }

            return result;
        }

        void UnlockAbilities(Settings::TheDeepDeepProfile* profile, const Ability::FindableAbilitySet& abilities)
        {
            for (const auto& characterType : abilities)
            {
                for (const auto& abilityType : characterType.second)
                {
                    profile->AddAbility(characterType.first, abilityType);
                }
            }
        }

        UnlockFraction ComputeUnlockedAbilityFraction(const Settings::TheDeepDeepProfile* profile, const Ability::FindableAbilitySet& abilities)
        {
            UnlockFraction result(0, 0);

            for (const auto& characterType : abilities)
            {
                for (const auto& abilityType : characterType.second)
                {
                    result.second++;
                    if (profile->IsAbilityAdded(characterType.first, abilityType))
                    {
                        result.first++;
                    }
                }
            }

            return result;
        }

        CampaignLevelCheckpoint::CampaignLevelCheckpoint()
        {
        }

        CampaignLevelCheckpoint::~CampaignLevelCheckpoint()
        {
        }

        const Splinef& CampaignLevelCheckpoint::GetCharacterSpawnArea() const
        {
            return _spawnArea;
        }

        void CampaignLevelCheckpoint::SetCharacterSpawnArea(const Splinef& spawnArea)
        {
            _spawnArea = spawnArea;
        }

        const std::string& CampaignLevelCheckpoint::GetNameCode() const
        {
            return _nameCode;
        }

        void CampaignLevelCheckpoint::SetNameCode(const std::string& name)
        {
            _nameCode = name;
        }

        bool CampaignLevelCheckpoint::WasCharacterKilled(LayerID layer, TriggerID spawnTrigger) const
        {
            auto iter = _killedCharacters.find(layer);
            if (iter == _killedCharacters.end())
            {
                return false;
            }

            return iter->second.count(spawnTrigger) > 0;
        }

        void CampaignLevelCheckpoint::SetCharacterKilled(LayerID layer, TriggerID spawnTrigger)
        {
            _killedCharacters[layer].insert(spawnTrigger);
        }

        bool CampaignLevelCheckpoint::WasPlayerCharacterKilled(uint32_t idx) const
        {
            return _killedPlayerCharacters.count(idx) > 0;
        }

        bool CampaignLevelCheckpoint::ShouldFlameHolderBeOn(LayerID layer, TriggerID spawnTrigger) const
        {
            auto layerIter = _flameHolderStates.find(layer);
            assert(layerIter != _flameHolderStates.end());
            auto flameHolderIter = layerIter->second.find(spawnTrigger);
            assert(flameHolderIter != layerIter->second.end());
            return flameHolderIter->second;
        }

        void CampaignLevelCheckpoint::SetFlameHolderOn(LayerID layer, TriggerID spawnTrigger, bool on)
        {
            _flameHolderStates[layer][spawnTrigger] = on;
        }

        void CampaignLevelCheckpoint::SetPlayerCharacterKilled(uint32_t idx)
        {
            _killedPlayerCharacters.insert(idx);
        }

        bool CampaignLevelCheckpoint::WasCheckpointReached(uint32_t checkpoint) const
        {
            return _reachedCheckpoints.count(checkpoint) > 0;
        }

        void CampaignLevelCheckpoint::SetCheckpointReached(uint32_t checkpoint)
        {
            _reachedCheckpoints.insert(checkpoint);
        }

        const std::vector<std::pair<Vector2f, Vector2f>>& CampaignLevelCheckpoint::GetGrapplePositions(LayerID layer) const
        {
            auto iter = _grapples.find(layer);
            if (iter != _grapples.end())
            {
                return iter->second;
            }
            else
            {
                static const std::vector<std::pair<Vector2f, Vector2f>> defaultPositions;
                return defaultPositions;
            }
        }

        void CampaignLevelCheckpoint::AddGrapplePosition(LayerID layer, const Vector2f& a, const Vector2f& b)
        {
            _grapples[layer].push_back(std::make_pair(a, b));
        }

        float CampaignLevelCheckpoint::GetBridgeBuildPercentage(LayerID layer, SplineID spline) const
        {
            auto layerIter = _bridges.find(layer);
            assert(layerIter != _bridges.end());
            auto splineIter = layerIter->second.find(spline);
            assert(splineIter != layerIter->second.end());
            return splineIter->second;
        }

        void CampaignLevelCheckpoint::SetBridgeBuildPercentage(LayerID layer, SplineID spline, float buildperc)
        {
            _bridges[layer][spline] = buildperc;
        }

        const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& CampaignLevelCheckpoint::GetCheckpointStates() const
        {
            return _checkpoints;
        }

        void CampaignLevelCheckpoint::SetCheckpointStates(const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& checkpoints)
        {
            _checkpoints = checkpoints;
        }

        bool CampaignLevelCheckpoint::WasTriggeredSwellPlayed(const std::string& name) const
        {
            return _playedTriggeredSwells.count(name) > 0;
        }

        void CampaignLevelCheckpoint::SetTriggeredSwellPlayed(const std::string& name)
        {
            _playedTriggeredSwells.insert(name);
        }

        CampaignLevel::CampaignLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : BasicLevel(parameters)
            , _campaignParameters(campaignParameters)
            , _playerController(nullptr)
            , _monsterController(nullptr)
            , _wildlifeController(nullptr)
        {
        }

        CampaignLevel::~CampaignLevel()
        {
        }

        void CampaignLevel::AddMusicTracks(const Audio::MusicTrackMap& tracks)
        {
            auto& musicManager = GetMusicManager();
            for (const auto& track : tracks)
            {
                musicManager.AddTrack(track.first, track.second);
                _musicTrackAreas.push_back(track.first);
            }
        }

        void CampaignLevel::AddTrackedTriggeredMusicSwell(const std::string& triggerName, const std::string& swellName, std::shared_ptr<const CampaignLevelCheckpoint> checkpoint)
        {
            if (checkpoint != nullptr && checkpoint->WasTriggeredSwellPlayed(triggerName))
            {
                _playedTriggeredMusicSwells.insert(triggerName);
            }
            else
            {
                _triggeredMusicSwells[triggerName].push_back(swellName);
            }
        }

        void CampaignLevel::OnCreate()
        {
            BasicLevel::OnCreate();

            _playerController = CreateController("player", Character::BindControllerConstructor<Character::Player>());
            _playerController->AddResources(_campaignParameters.StartingResources);

            std::shared_ptr<const CampaignLevelCheckpoint> checkpoint = GetCheckpoint();

            Splinef spawnArea = (checkpoint != nullptr) ? checkpoint->GetCharacterSpawnArea() : GetCharacterSpawnArea();
            for (uint32_t i = 0; i < _campaignParameters.Characters.size(); i++)
            {
                if (checkpoint != nullptr && checkpoint->WasPlayerCharacterKilled(i))
                {
                    continue;
                }

                float perc = _campaignParameters.Characters.size() > 1 ? (i / float(_campaignParameters.Characters.size() - 1)) : 0.5f;
                Vector2f spawnPos = spawnArea.Evalulate(perc);

                const CampaignLevelCharacterInfo& characterInfo = _campaignParameters.Characters[i];
                Character::Character* character = GetPrimaryLayer()->SpawnCharacter(spawnPos, "dwarf", _playerController,
                    characterInfo.Constructor);

                for (uint32_t j = 0; j < characterInfo.Items.size(); j++)
                {
                    character->GiveItem(characterInfo.Items[j]);
                }

                for (uint32_t j = 0; j < characterInfo.Abilities.size(); j++)
                {
                    character->AddAbility(characterInfo.Abilities[j]);
                }

                _playerCharacters[i] = character->GetID();
                OnSpawnCampaignCharacter(character);
            }

            Vector2f initialCameraPos;
            if (checkpoint != nullptr)
            {
                for (uint32_t layerIdx = 0; layerIdx < GetLayerCount(); layerIdx++)
                {
                    LevelLayerInstance* layer = GetLayer(layerIdx);

                    const auto& grapples = checkpoint->GetGrapplePositions(layer->GetID());
                    for (const auto& grapplePos : grapples)
                    {
                        Character::CharacterConstructor<Character::GrappleRope> constructor =
                            Character::BindCharacterConstructor<Character::GrappleRope>(grapplePos.first, grapplePos.first, grapplePos.second, 0.0f);
                        layer->SpawnCharacter(grapplePos.first, "grapple", nullptr, constructor);
                    }
                }

                initialCameraPos = spawnArea.Bounds().Middle();
            }
            else
            {
                initialCameraPos = GetPrimaryLayer()->GetTriggerPosition("camera_start");
            }

            _monsterController = CreateController("monster_controller", Character::BindControllerConstructor<Character::MonsterController>());
            _wildlifeController = CreateController("wildlife_controller", Character::BindControllerConstructor<Character::WildlifeController>());

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                auto layer = GetLayer(i);
                assert(layer->GetID() == i);
                for (const auto& aggroZone : layer->GetTriggerAreas("aggro_zone"))
                {
                    _aggroZones[layer->GetID()].push_back(aggroZone);
                }
            }

            GetCameraController().Pan(initialCameraPos);
            SetTargetCameraViewSize(2500.0f, 16.0f / 9.0f);

            SetDiscoveryEnabled(true);
        }

        static std::vector<Character::Character*> GetCharactersInPolygons(const Character::Controller* controller, const std::vector<Polygonf>& polygons)
        {
            assert(!polygons.empty());
            return controller->GetCharacters<Character::Character>([&polygons](const Character::Character* character)
            {
                for (const auto& poly : polygons)
                {
                    if (Polygonf::Contains(poly, character->GetPosition()))
                    {
                        return true;
                    }
                }

                return false;
            });
        }

        void CampaignLevel::OnUpdate(double totalTime, float dt)
        {
            const LevelLayerInstance* primaryLayer = GetPrimaryLayer();

            const Vector2f& camPos = primaryLayer->GetCamera().GetPosition();
            Audio::MusicManager& musicManager = GetMusicManager();
            for (const auto& musicTrackArea : _musicTrackAreas)
            {
                std::vector<Character::Character*> characters = GetCharactersInPolygons(_playerController, primaryLayer->GetTriggerAreas(musicTrackArea));
                float minDistSq = std::numeric_limits<float>::max();
                for (const auto& character : characters)
                {
                    minDistSq = Min(minDistSq, Vector2f::DistanceSquared(camPos, character->GetPosition()));
                }

                musicManager.SetTrackWeightings(musicTrackArea, Sqrt(minDistSq));
            }

            auto triggeredMusicSwellIter = _triggeredMusicSwells.begin();
            while (triggeredMusicSwellIter != _triggeredMusicSwells.end())
            {
                const auto& triggerName = triggeredMusicSwellIter->first;
                std::vector<Character::Character*> characters = GetCharactersInPolygons(_playerController, primaryLayer->GetTriggerAreas(triggerName));
                if (!characters.empty())
                {
                    for (const auto& swell : triggeredMusicSwellIter->second)
                    {
                        PlayMusicSwell(swell);
                    }
                    _playedTriggeredMusicSwells.insert(triggerName);
                    triggeredMusicSwellIter = _triggeredMusicSwells.erase(triggeredMusicSwellIter);
                }
                else
                {
                    triggeredMusicSwellIter++;
                }
            }

            for (auto& aggroZoneLayer : _aggroZones)
            {
                auto layer = GetLayer(aggroZoneLayer.first);

                auto iter = aggroZoneLayer.second.begin();
                while (iter != aggroZoneLayer.second.end())
                {
                    const Polygonf& area = *iter;

                    CharacterFilterFunction<Character::Character> filterFunc = [&](const Character::Character* character)
                    {
                        return character->GetLevelLayer() == layer && Polygonf::Contains(area, character->GetPosition());
                    };

                    std::vector<Character::Character*> charactersInZone = _playerController->GetCharacters(filterFunc);
                    if (!charactersInZone.empty())
                    {
                        std::vector<Character::Character*> monstersInZone = _monsterController->GetCharacters(filterFunc);
                        Character::Action action = Character::CreateWeakAttackAction(Random::RandomItem(charactersInZone)->GetID());
                        for (auto monster : monstersInZone)
                        {
                            if (monster->GetCurrentState() == Character::CharacterState_Idle)
                            {
                                monster->PushAction(action, false);
                            }
                        }

                        iter = aggroZoneLayer.second.erase(iter);
                    }
                    else
                    {
                        iter++;
                    }
                }
            }

            BasicLevel::OnUpdate(totalTime, dt);
        }

        Splinef CampaignLevel::GetCharacterSpawnArea() const
        {
            Splinef defaultSpawnArea;
            defaultSpawnArea.AddPoint(GetPrimaryLayer()->GetTriggerPosition("camera_start"));
            return defaultSpawnArea;
        }

        void CampaignLevel::OnSpawnCampaignCharacter(Character::Character* character)
        {
        }

        Character::Controller* CampaignLevel::GetPlayerController() const
        {
            return _playerController;
        }

        Character::Controller* CampaignLevel::GetMonsterController() const
        {
            return _monsterController;
        }

        Character::Controller* CampaignLevel::GetWildlifeController() const
        {
            return _wildlifeController;
        }

        void CampaignLevel::CompleteLevel(bool success)
        {
            if (success)
            {
                Settings::TheDeepDeepProfile* profile = GetProfile<Settings::TheDeepDeepProfile>();
                assert(profile);

                OnLevelSuccessfullyCompleted(profile);

                _playerController->OnLevelSuccessfullyCompleted(profile);

                _campaignParameters.SaveProfile();

                _playerController->OnLevelVictory();
            }
            else
            {
                _playerController->OnLevelFailed();
            }
        }

        void CampaignLevel::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
        }

        void CampaignLevel::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> checkpoint)
        {
            for (const auto& spawnLayer : _spawnedCharacters)
            {
                LayerID layerID = spawnLayer.first;
                LevelLayerInstance* layer = GetLayer(layerID);
                for (const auto& spawnCharacter : spawnLayer.second)
                {
                    Character::CharacterID characterID = spawnCharacter.second;
                    const Character::Character* character = layer->GetCharacter(characterID);
                    if (character == nullptr)
                    {
                        TriggerID triggerID = spawnCharacter.first;
                        checkpoint->SetCharacterKilled(layerID, triggerID);
                    }
                }
            }

            for (const auto& spawnLayer : _spawnedFlameHolders)
            {
                LayerID layerID = spawnLayer.first;
                LevelLayerInstance* layer = GetLayer(layerID);
                for (const auto& spawnFlameHolder : spawnLayer.second)
                {
                    Character::CharacterID characterID = spawnFlameHolder.second;
                    const Character::FlameHolder* flameHolder = layer->GetCharacter<Character::FlameHolder>(characterID);
                    if (flameHolder != nullptr)
                    {
                        TriggerID triggerID = spawnFlameHolder.first;
                        checkpoint->SetFlameHolderOn(layerID, triggerID, flameHolder->IsOn());
                    }
                }
            }

            for (const auto& spawnLayer : _spawnedBridges)
            {
                LayerID layerID = spawnLayer.first;
                LevelLayerInstance* layer = GetLayer(layerID);
                for (const auto& spawnBridge : spawnLayer.second)
                {
                    Character::CharacterID characterID = spawnBridge.second;
                    const Character::Bridge2* bridge = layer->GetCharacter<Character::Bridge2>(characterID);
                    if (bridge != nullptr)
                    {
                        SplineID splineID = spawnBridge.first;
                        checkpoint->SetBridgeBuildPercentage(layerID, splineID, bridge->GetBuildPercentage());
                    }
                }
            }

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            for (const auto& playerCharacter : _playerCharacters)
            {
                Character::CharacterID characterID = playerCharacter.second;
                const Character::Character* character = primaryLayer->GetCharacter(characterID);
                if (character == nullptr)
                {
                    uint32_t playerCharacterIdx = playerCharacter.first;
                    checkpoint->SetPlayerCharacterKilled(playerCharacterIdx);
                }
            }

            for (uint32_t checkpointIdx = 0; checkpointIdx < _checkpointStates.size(); checkpointIdx++)
            {
                if (_checkpointStates[checkpointIdx] != nullptr)
                {
                    checkpoint->SetCheckpointReached(checkpointIdx);
                }
            }

            for (uint32_t layerIdx = 0; layerIdx < GetLayerCount(); layerIdx++)
            {
                const LevelLayerInstance* layer = GetLayer(layerIdx);

                std::vector<Character::GrappleRope*> grapples =
                    layer->GetCharacters<Character::GrappleRope>([this](const Character::GrappleRope* rope){ return rope->GetController() == GetPlayerController(); });
                for (const auto& grapple : grapples)
                {
                    checkpoint->AddGrapplePosition(layer->GetID(), grapple->GetAnchor(), grapple->GetDestination());
                }
            }

            for (const auto& playedTriggerdSwell : _playedTriggeredMusicSwells)
            {
                checkpoint->SetTriggeredSwellPlayed(playedTriggerdSwell);
            }
        }

        std::vector<Character::FlameHolder*> CampaignLevel::SpawnTrackedFlameHoldersAtTriggers(const std::string& triggerLocations,
                                                                                               Character::CharacterConstructor<Character::FlameHolder> constructor,
                                                                                               const std::string& nameBase,
                                                                                               std::shared_ptr<const CampaignLevelCheckpoint> checkpoint)
        {
            std::vector<Character::FlameHolder*> result;

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                LevelLayerInstance* layer = GetLayer(i);
                std::vector<TriggerID> ids;
                std::vector<Vector2f> spawns = layer->GetTriggerPositions(triggerLocations, &ids);
                for (uint32_t j = 0; j < spawns.size(); j++)
                {
                    Character::FlameHolder* torch = layer->SpawnCharacter(spawns[j], Format("%s_%u", nameBase.c_str(), j), nullptr, constructor);
                    _spawnedFlameHolders[layer->GetID()][ids[j]] = torch->GetID();
                    result.push_back(torch);

                    if (checkpoint != nullptr)
                    {
                        torch->SetOn(checkpoint->ShouldFlameHolderBeOn(layer->GetID(), ids[j]));
                    }
                }
            }

            return result;
        }

        static Character::CharacterConstructor<Character::Bridge2> BindBridgeConstructor(LevelLayerInstance* layer, const Splinef& location, float buildPercent, const Item::Resources& buildCost)
        {
            assert(layer != nullptr && location.Size() >= 2);

            std::shared_ptr<Pathfinding::PathPosition> a = layer->FindLedge(location.At(0), 500.0f, Pathfinding::EdgeType_Walk);
            std::shared_ptr<Pathfinding::PathPosition> b = layer->FindLedge(location.At(location.Size() - 1), 500.0f, Pathfinding::EdgeType_Walk);

            assert(a != nullptr && b != nullptr);
            return Character::BindCharacterConstructor<Character::Bridge2>(a->GetPosition(), b->GetPosition(), buildPercent, buildCost);
        }

        std::vector<Character::Bridge2*> CampaignLevel::SpawnTrackedBridgesAtSplines(
            const std::string& splineLocations, float buildPerc, const Item::Resources& buildCost,
            std::shared_ptr<const CampaignLevelCheckpoint> checkpoint)
        {
            std::vector<Character::Bridge2*> result;

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                LevelLayerInstance* layer = GetLayer(i);
                std::vector<SplineID> ids;
                std::vector<Splinef> spawns = layer->GetSplines(splineLocations, &ids);
                for (uint32_t j = 0; j < spawns.size(); j++)
                {
                    float finalBuildPerc = checkpoint ? checkpoint->GetBridgeBuildPercentage(layer->GetID(), ids[j]) : buildPerc;
                    auto constructor = BindBridgeConstructor(layer, spawns[j], finalBuildPerc, buildCost);
                    auto bridge = layer->SpawnCharacter(spawns[j][0], "bridge", nullptr, constructor);
                    _spawnedBridges[layer->GetID()][ids[j]] = bridge->GetID();
                    result.push_back(bridge);
                }
            }

            return result;
        }

        const std::vector<std::shared_ptr<CampaignLevelCheckpoint>>& CampaignLevel::GetCheckpoints() const
        {
            return _checkpointStates;
        }

        void CampaignLevel::RestartAtCheckpoint(uint32_t checkpoint)
        {
            assert(checkpoint < _checkpointStates.size());
            assert(_checkpointStates[checkpoint] != nullptr);

            _checkpointStates[checkpoint]->SetCheckpointStates(_checkpointStates);
            _campaignParameters.CheckpointFunction(_checkpointStates[checkpoint]);
        }

        void CampaignLevel::RestartAtLoadout()
        {
            _campaignParameters.LoadoutFunction();
        }

        void CampaignLevel::TransitionToCampaignMenu()
        {
            _campaignParameters.MenuFunction();
        }
    }

    template <>
    void EnumeratePreloads<Level::CampaignLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);

        EnumeratePreloads<Character::Player>(preloads);
        EnumeratePreloads<Character::MonsterController>(preloads);
        EnumeratePreloads<Character::WildlifeController>(preloads);

        EnumeratePreloads<Character::Checkpoint>(preloads);
        EnumeratePreloads<Character::GrappleRope>(preloads);
        EnumeratePreloads<Character::Bridge>(preloads);
        EnumeratePreloads<Character::Bridge2>(preloads);
    }
}
