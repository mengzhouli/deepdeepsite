#include "Levels/DwarfHomes/DwarfHomeLevel.hpp"

#include "Characters/Bridge.hpp"
#include "Characters/Ladder.hpp"
#include "Characters/Torch.hpp"
#include "Characters/GrappleRope.hpp"

#include "Characters/Gobbo.hpp"
#include "Items/Weapons/GobboWeapons.hpp"

#include "Characters/Critter.hpp"

#include "Characters/Door.hpp"
#include "Characters/Lever.hpp"
#include "Characters/DestructableRocks.hpp"
#include "Characters/ResourceNode.hpp"
#include "Characters/Chest.hpp"

#include "NavigationUtility.hpp"

namespace Dwarf
{
    namespace Level
    {
        DwarfHomeCheckpoint::DwarfHomeCheckpoint()
        {
        }

        DwarfHomeCheckpoint::~DwarfHomeCheckpoint()
        {
        }

        bool DwarfHomeCheckpoint::GetDoorState(LayerID layer, TriggerID trigger) const
        {
            auto layerIter = _doorStates.find(layer);
            assert(layerIter != _doorStates.end());
            auto doorIter = layerIter->second.find(trigger);
            assert(doorIter != layerIter->second.end());
            return doorIter->second;
        }

        void DwarfHomeCheckpoint::SetDoorState(LayerID layer, TriggerID trigger, bool state)
        {
            _doorStates[layer][trigger] = state;
        }

        Item::Resources DwarfHomeCheckpoint::GetOreState(LayerID layer, TriggerID trigger) const
        {
            auto layerIter = _oreStates.find(layer);
            if (layerIter == _oreStates.end())
            {
                return Item::Resources();
            }

            auto oreIter = layerIter->second.find(trigger);
            if (oreIter == layerIter->second.end())
            {
                return Item::Resources();
            }

            return oreIter->second;
        }

        void DwarfHomeCheckpoint::SetOreState(LayerID layer, TriggerID trigger, const Item::Resources& state)
        {
            _oreStates[layer][trigger] = state;
        }

        bool DwarfHomeCheckpoint::WasChestOpened(LayerID layer, TriggerID trigger) const
        {
            auto layerIter = _chestStates.find(layer);
            if (layerIter == _chestStates.end())
            {
                return true;
            }

            auto chestIter = layerIter->second.find(trigger);
            if (chestIter == layerIter->second.end())
            {
                return true;
            }

            return chestIter->second;
        }

        void DwarfHomeCheckpoint::SetChestOpened(LayerID layer, TriggerID trigger, bool opened)
        {
            _chestStates[layer][trigger] = opened;
        }


        static const std::map<Audio::EnvironmentType, std::pair<Audio::SoundPathVector, float>> DwarfHomeAmbientEnvironementSoundPaths
        {
            {
                Audio::EnvironmentType::Cave,
                {
                    {
                        { "Audio/Ambiance/Ambience_CaveDrips3_Long_Stereo.ogg", 1.0f },
                    },
                    1.0f
                },
            },
            {
                Audio::EnvironmentType::StoneTunnel,
                {
                    {
                        { "Audio/Ambiance/Ambience_CaveDrips3_Long_Stereo.ogg", 1.0f },
                    },
                    1.0f
                },
            },
        };

        DwarfHomeLevel::DwarfHomeLevel(const LevelParameters& parameters, const CampaignLevelParameters& campaignParameters)
            : CampaignLevel(parameters, campaignParameters)
        {
            for (const auto& environmentSounds : DwarfHomeAmbientEnvironementSoundPaths)
            {
                AddAmbientSounds(environmentSounds.first, environmentSounds.second.first, environmentSounds.second.second);
            }
        }

        DwarfHomeLevel::~DwarfHomeLevel()
        {
        }

        std::vector<Character::CharacterID> DwarfHomeLevel::SpawnLadders(LevelLayerInstance* layer, const std::string& splineName)
        {
            std::vector<Character::CharacterID> result;

            std::vector<Splinef> ladderSpawns = layer->GetSplines(splineName);
            for (uint32_t j = 0; j < ladderSpawns.size(); j++)
            {
                result.push_back(layer->SpawnCharacter(ladderSpawns[j].Bounds().Middle(), "ladder", nullptr, BindLadderConstructor(layer, ladderSpawns[j]))->GetID());
            }

            return result;
        }

        std::vector<Character::CharacterID> DwarfHomeLevel::SpawnGrapples(LevelLayerInstance* layer, const std::string& splineName)
        {
            std::vector<Character::CharacterID> result;

            std::vector<Splinef> ladderSpawns = layer->GetSplines(splineName);
            for (const auto& ladderSpawn : ladderSpawns)
            {
                result.push_back(layer->SpawnCharacter(ladderSpawn.Bounds().Middle(), "grapple", nullptr, BindGrappleConstructor(layer, ladderSpawn))->GetID());
            }

            return result;
        }

        Character::CharacterID DwarfHomeLevel::SpawnTrackedDoor(LevelLayerInstance* layer, const std::string& leverSpawnName, const std::string& doorSpawnName,
                                                                const std::string& collisionClosedName, const std::string& collisionOpenName, Character::DoorSize size, bool startOpen, bool inverted,
                                                                Character::OpenStateChangeCallback stateChangeCallback, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            TriggerID triggerID;
            const Vector2f position = layer->GetTriggerPosition(doorSpawnName, &triggerID);

            bool open = (checkpoint != nullptr) ? checkpoint->GetDoorState(layer->GetID(), triggerID) : startOpen;

            Character::CharacterConstructor<Character::Door> doorConstructor =
                Character::BindCharacterConstructor<Character::Door>(size, collisionClosedName, collisionOpenName, open, inverted, stateChangeCallback);
            Character::Door* doorCharacter = layer->SpawnCharacter(position, "door", nullptr, doorConstructor);
            Character::CharacterID doorID = doorCharacter->GetID();

            Character::OnLeverUseCallback leverCallback = [layer, doorID](Character::Character*, bool open)
            {
                Character::Door* door = layer->GetCharacter<Character::Door>(doorID);
                assert(door != nullptr);
                door->SetOpen(!open);
            };

            Character::CharacterConstructor<Character::Lever> leverConstructor =
                Character::BindCharacterConstructor<Character::Lever>(leverCallback, open);
            Character::Character* leverCharacter = layer->SpawnCharacter(layer->GetTriggerPosition(leverSpawnName), "lever", nullptr, leverConstructor);

            _spawnedDoors[layer->GetID()][triggerID] = doorID;

            return doorID;
        }

        Character::CharacterID DwarfHomeLevel::SpawnTrackedRubble(LevelLayerInstance* layer, Character::RubbleSize size, const std::string& triggerName, const std::string& collisionName,
                                                                   Character::OnDestructionCallback callback, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            Character::CharacterConstructor<Character::DestructableRubble> rubbleConstructor =
                Character::BindCharacterConstructor<Character::DestructableRubble>(size, collisionName, callback);
            std::vector<Character::DestructableRubble*> characters = SpawnTrackedCharactersAtTriggers(triggerName, rubbleConstructor, "rubble", nullptr, checkpoint);
            assert(characters.empty() || characters.size() == 1);
            if (characters.empty())
            {
                layer->SetTerrainCollisionEnabled(collisionName, false);
                return 0;
            }
            else
            {
                return characters[0]->GetID();
            }
        }

        Character::CharacterID DwarfHomeLevel::SpawnTrackedDestructableWall(LevelLayerInstance* layer, const std::string& triggerName, const std::string& collisionName, Character::OnDestructionCallback callback, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            Character::CharacterConstructor<Character::DestructableWall> rubbleConstructor =
                Character::BindCharacterConstructor<Character::DestructableWall>(collisionName, callback);
            std::vector<Character::DestructableWall*> characters = SpawnTrackedCharactersAtTriggers(triggerName, rubbleConstructor, "destructable_wall", nullptr, checkpoint);
            assert(characters.empty() || characters.size() == 1);
            if (characters.empty())
            {
                layer->SetTerrainCollisionEnabled(collisionName, false);
                return 0;
            }
            else
            {
                return characters[0]->GetID();
            }
        }

        Character::CharacterID DwarfHomeLevel::SpawnTrackedOre(LevelLayerInstance* layer, const Vector2f& position, TriggerID triggerID, const Item::Resources& resources,
            std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            const Item::Resources& realResources = (checkpoint != nullptr) ? checkpoint->GetOreState(layer->GetID(), triggerID) : resources;
            if (realResources == Item::Resources())
            {
                return 0;
            }

            Character::CharacterConstructor<Character::GoldNode> oreConstructor = Character::BindCharacterConstructor<Character::GoldNode>(realResources);
            Character::GoldNode* oreCharacter = layer->SpawnCharacter(position, "ore", nullptr, oreConstructor);

            _spawnedOres[layer->GetID()][triggerID] = oreCharacter->GetID();

            return oreCharacter->GetID();
        }

        std::vector<Character::CharacterID> DwarfHomeLevel::SpawnTrackedOres(LevelLayerInstance* layer, const std::string& triggerName, const Item::Resources& resources,
                                                                             std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            std::vector<Character::CharacterID> result;

            std::vector<TriggerID> ids;
            std::vector<Vector2f> oreSpawns = layer->GetTriggerPositions(triggerName, &ids);
            for (uint32_t i = 0; i < oreSpawns.size(); i++)
            {
                Character::CharacterID id = SpawnTrackedOre(layer, oreSpawns[i], ids[i], resources, checkpoint);
                if (id != 0)
                {
                    result.push_back(id);
                }
            }

            return result;
        }

        Character::CharacterID DwarfHomeLevel::SpawnTrackedChest(LevelLayerInstance* layer, const std::string& spawnTrigger, bool invertX,
                                                                 const Item::FindableItemSet& items, std::shared_ptr<const DwarfHomeCheckpoint> checkpoint)
        {
            LayerID layerID = layer->GetID();
            TriggerID triggerID = 0;
            Vector2f position = layer->GetTriggerPosition(spawnTrigger, &triggerID);

            Item::FindableItemSet remainingItems = ComputeRemainingFindableItems(GetProfile<Settings::TheDeepDeepProfile>(), items);

            bool alreadyHadItems = ComputeItemCount(remainingItems) == 0;
            bool chestOpenedByCheckpoint = (checkpoint != nullptr && checkpoint->WasChestOpened(layerID, triggerID));
            bool opened = alreadyHadItems || chestOpenedByCheckpoint;
            _openedChests[layerID][triggerID] = opened;

            Character::OnChestOpenCallback callback = nullptr;
            if (chestOpenedByCheckpoint)
            {
                _foundItems = Item::MergeFindableItems(_foundItems, remainingItems);
                remainingItems = Item::FindableItemSet();
            }
            else if (alreadyHadItems)
            {
            }
            else
            {
                callback = [=](Character::Chest* chest)
                {
                    _openedChests[layerID][triggerID] = true;
                    _foundItems = Item::MergeFindableItems(_foundItems, chest->GetFindableItems());
                };
            }

            const auto& constructor = Character::BindCharacterConstructor<Character::IronChest>(remainingItems, opened, callback);
            auto chest = layer->SpawnCharacter(position, "chest", nullptr, constructor);
            chest->SetRotation(invertX ? Rotatorf::Pi : Rotatorf::Zero);
            return chest->GetID();
        }

        void DwarfHomeLevel::OnCreate()
        {
            CampaignLevel::OnCreate();

            SetDefaultEnvironmenType(Audio::EnvironmentType::Cave);
            SetTriggerAreaEnvironmenType("audio_area_none", Audio::EnvironmentType::None);
            SetTriggerAreaEnvironmenType("audio_area_cave", Audio::EnvironmentType::Cave);
            SetTriggerAreaEnvironmenType("audio_area_stone_tunnel", Audio::EnvironmentType::StoneTunnel);
            SetTriggerAreaEnvironmenType("audio_area_stone_room", Audio::EnvironmentType::StoneRoom);
            SetTriggerAreaEnvironmenType("audio_area_wood_room", Audio::EnvironmentType::WoodRoom);

            std::shared_ptr<const CampaignLevelCheckpoint> checkpoint = GetCheckpoint();

            SpawnTrackedFlameHoldersAtTriggers("brazier_spawn", Character::BindCharacterConstructor<Character::PotBrazier>(BaseFireColor, true, true, true), "brazier", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("hanging_brazier_spawn", Character::BindCharacterConstructor<Character::HangingBrazier>(BaseFireColor, true, true, true), "hanging_brazier", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("campfire_spawn", Character::BindCharacterConstructor<Character::CampFire>(BaseFireColor, true, true, true), "campfire", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("cookfire_spawn", Character::BindCharacterConstructor<Character::CookFire>(BaseFireColor, true, true, true), "cookfire", checkpoint);

            SpawnTrackedFlameHoldersAtTriggers("brazier_spawn_off", Character::BindCharacterConstructor<Character::PotBrazier>(BaseFireColor, true, false, true), "brazier", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("hanging_brazier_spawn_off", Character::BindCharacterConstructor<Character::HangingBrazier>(BaseFireColor, true, false, true), "hanging_brazier", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("campfire_spawn_off", Character::BindCharacterConstructor<Character::CampFire>(BaseFireColor, true, false, true), "campfire", checkpoint);
            SpawnTrackedFlameHoldersAtTriggers("cookfire_spawn_off", Character::BindCharacterConstructor<Character::CookFire>(BaseFireColor, true, false, true), "cookfire", checkpoint);

            static const Item::Resources BridgeCost(20);
            SpawnTrackedBridgesAtSplines("bridge_spawn", 1.0f, BridgeCost, checkpoint);
            SpawnTrackedBridgesAtSplines("broken_bridge_spawn", 0.0f, BridgeCost, checkpoint);

            for (uint32_t i = 0; i < GetLayerCount(); i++)
            {
                LevelLayerInstance* layer = GetLayer(i);

                SpawnLadders(layer, "ladder_spawn");
                SpawnGrapples(layer, "grapple_rope_spawn");

                std::vector<Splinef> ratSpawns = layer->GetSplines("rat_spawn");
                for (const Splinef& ratSpawn : ratSpawns)
                {
                    Vector2f spawnPosition = ratSpawn.Evalulate(Random::RandomBetween(0.0f, 1.0f));
                    layer->SpawnCharacter(spawnPosition, "rat", GetWildlifeController(), Character::BindCharacterConstructor<Character::Rat>(ratSpawn));
                }
            }

            // Spawn gobbos
            std::vector< Item::ItemConstructor<Item::Weapon> > meleeGobboWeapons =
            {
                Item::BindItemConstructor<Item::GobBlade>(),
                Item::BindItemConstructor<Item::GobDagger>(),
                Item::BindItemConstructor<Item::GobFork>(),
                Item::BindItemConstructor<Item::GobShank>(),
                Item::BindItemConstructor<Item::GobTrident>(),
                Item::BindItemConstructor<Item::GobSword>(),
            };
            std::vector< Item::ItemConstructor<Item::Weapon> > meleeGobboChiefWeapons =
            {
                Item::BindItemConstructor<Item::GobJawBlade>(),
                Item::BindItemConstructor<Item::GobSkullClub>(),
                Item::BindItemConstructor<Item::GobOrkAxe>(),
                Item::BindItemConstructor<Item::GobOrkSickle>(),
            };
            std::vector< Item::ItemConstructor<Item::Weapon> > rangedGobboWeapons =
            {
                Item::BindItemConstructor<Item::GobBow>(),
            };
            std::vector< Item::ItemConstructor<Item::Weapon> > throwGobboWeapons =
            {
                Item::BindItemConstructor<Item::GobThrowingRocks>(),
            };

            Character::CharacterConstructor<Character::Gobbo> meleeGobboConstructor = Character::BindCharacterConstructor<Character::Gobbo>();
            std::vector<Character::Gobbo*> meleeGobbos = SpawnTrackedCharactersAtTriggers("monster_spawn", meleeGobboConstructor, "monster", GetMonsterController(), checkpoint);
            for (auto gobbo : meleeGobbos)
            {
                gobbo->GiveItem(Random::RandomItem(meleeGobboWeapons));
            }

            Character::CharacterConstructor<Character::GobboThrower> throwingGobboConstructor = Character::BindCharacterConstructor<Character::GobboThrower>();
            std::vector<Character::GobboThrower*> throwingGobbos = SpawnTrackedCharactersAtTriggers("throw_monster_spawn", throwingGobboConstructor, "monster", GetMonsterController(), checkpoint);
            for (auto gobbo : throwingGobbos)
            {
                gobbo->GiveItem(Random::RandomItem(throwGobboWeapons));
            }

            Character::CharacterConstructor<Character::GobboBowman> rangedGobboConstructor = Character::BindCharacterConstructor<Character::GobboBowman>();
            std::vector<Character::GobboBowman*> rangedGobbos = SpawnTrackedCharactersAtTriggers("ranged_monster_spawn", rangedGobboConstructor, "monster", GetMonsterController(), checkpoint);
            for (auto gobbo : rangedGobbos)
            {
                gobbo->GiveItem(Random::RandomItem(rangedGobboWeapons));
            }

            Character::CharacterConstructor<Character::GobboBomber> gobboBomberConstructor = Character::BindCharacterConstructor<Character::GobboBomber>();
            std::vector<Character::GobboBomber*> bomberGobbos = SpawnTrackedCharactersAtTriggers("bomb_monster_spawn", gobboBomberConstructor, "monster", GetMonsterController(), checkpoint);

            Character::CharacterConstructor<Character::GobboChief> gobboChiefConstructor = Character::BindCharacterConstructor<Character::GobboChief>();
            std::vector<Character::GobboChief*> chiefGobbos = SpawnTrackedCharactersAtTriggers("chief_monster_spawn", gobboChiefConstructor, "monster", GetMonsterController(), checkpoint);
            for (auto chief : chiefGobbos)
            {
                chief->GiveItem(Random::RandomItem(meleeGobboChiefWeapons));
            }

            Character::CharacterConstructor<Character::GobboTower> gobboTowerConstructor = Character::BindCharacterConstructor<Character::GobboTower>();
            std::vector<Character::GobboTower*> towers = SpawnTrackedCharactersAtTriggers("tower_spawn", gobboTowerConstructor, "tower", GetMonsterController(), checkpoint);
            for (auto rangedGobbo : Merge<Character::Character*>(rangedGobbos, throwingGobbos))
            {
                Character::GobboTower* closestTower = nullptr;
                float closestTowerDist = rangedGobbo->GetAggroRange();
                for (auto tower : towers)
                {
                    float dist = Vector2f::Distance(rangedGobbo->GetPosition(), tower->GetPosition());
                    if (dist < closestTowerDist)
                    {
                        closestTower = tower;
                        closestTowerDist = dist;
                    }
                }

                if (closestTower != nullptr)
                {
                    rangedGobbo->PushAction(Character::CreateInteractCharacterAction(closestTower->GetID()), false);
                }
            }
        }

        Splinef DwarfHomeLevel::GetCharacterSpawnArea() const
        {
            return GetPrimaryLayer()->GetSpline("dwarf_spawn");
        }

        void DwarfHomeLevel::PopulateCheckpoint(std::shared_ptr<CampaignLevelCheckpoint> baseCheckpoint)
        {
            std::shared_ptr<DwarfHomeCheckpoint> checkpoint = AsA<DwarfHomeCheckpoint>(baseCheckpoint);

            for (const auto& spawnLayer : _spawnedDoors)
            {
                LayerID layerID = spawnLayer.first;
                LevelLayerInstance* layer = GetLayer(layerID);
                for (const auto& spawnDoor : spawnLayer.second)
                {
                    Character::CharacterID characterID = spawnDoor.second;
                    const Character::Door* door = layer->GetCharacter<Character::Door>(characterID);
                    if (door != nullptr)
                    {
                        TriggerID triggerID = spawnDoor.first;
                        checkpoint->SetDoorState(layerID, triggerID, door->IsOpen());
                    }
                }
            }

            for (const auto& spawnLayer : _spawnedOres)
            {
                LayerID layerID = spawnLayer.first;
                LevelLayerInstance* layer = GetLayer(layerID);
                for (const auto& spawnOre : spawnLayer.second)
                {
                    Character::CharacterID characterID = spawnOre.second;
                    const Character::GoldNode* ore = layer->GetCharacter<Character::GoldNode>(characterID);
                    if (ore != nullptr)
                    {
                        TriggerID triggerID = spawnOre.first;
                        checkpoint->SetOreState(layerID, triggerID, ore->GetResources());
                    }
                }
            }

            for (const auto& spawnLayer : _openedChests)
            {
                LayerID layerID = spawnLayer.first;
                for (const auto& spawnChest : spawnLayer.second)
                {
                    TriggerID triggerID = spawnChest.first;
                    bool opened = spawnChest.second;
                    checkpoint->SetChestOpened(layerID, triggerID, opened);
                }
            }

            CampaignLevel::PopulateCheckpoint(checkpoint);
        }

        void DwarfHomeLevel::OnLevelSuccessfullyCompleted(Settings::TheDeepDeepProfile* profile)
        {
            CampaignLevel::OnLevelSuccessfullyCompleted(profile);
            UnlockItems(profile, _foundItems);
        }
    }

    template <>
    void EnumeratePreloads<Level::DwarfHomeLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::CampaignLevel>(preloads);

        EnumeratePreloads<Character::Lever>(preloads);
        EnumeratePreloads<Character::Door>(preloads);

        EnumeratePreloads<Character::IronChest>(preloads);

        EnumeratePreloads<Character::HangingBrazier>(preloads);
        EnumeratePreloads<Character::PotBrazier>(preloads);
        EnumeratePreloads<Character::CampFire>(preloads);
        EnumeratePreloads<Character::CookFire>(preloads);

        EnumeratePreloads<Character::Bridge>(preloads);
        EnumeratePreloads<Character::Ladder>(preloads);
        EnumeratePreloads<Character::GrappleRope>(preloads);
        EnumeratePreloads<Character::DestructableRubble>(preloads);
        EnumeratePreloads<Character::DestructableWall>(preloads);
        EnumeratePreloads<Character::GoldNode>(preloads);

        EnumeratePreloads<Character::GoldNode>(preloads);
        EnumeratePreloads<Item::GobBlade>(preloads);
        EnumeratePreloads<Item::GobDagger>(preloads);
        EnumeratePreloads<Item::GobFork>(preloads);
        EnumeratePreloads<Item::GobShank>(preloads);
        EnumeratePreloads<Item::GobSword>(preloads);
        EnumeratePreloads<Item::GobTrident>(preloads);
        EnumeratePreloads<Item::GobJawBlade>(preloads);
        EnumeratePreloads<Item::GobSkullClub>(preloads);
        EnumeratePreloads<Item::GobOrkAxe>(preloads);
        EnumeratePreloads<Item::GobOrkSickle>(preloads);
        EnumeratePreloads<Item::GobBow>(preloads);
        EnumeratePreloads<Item::GobThrowingRocks>(preloads);

        EnumeratePreloads<Character::Gobbo>(preloads);
        EnumeratePreloads<Character::GobboThrower>(preloads);
        EnumeratePreloads<Character::GobboBowman>(preloads);
        EnumeratePreloads<Character::GobboBomber>(preloads);
        EnumeratePreloads<Character::GobboChief>(preloads);
        EnumeratePreloads<Character::GobboTower>(preloads);

        EnumeratePreloads<Character::Rat>(preloads);

        for (const auto& environmentSounds : Level::DwarfHomeAmbientEnvironementSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, environmentSounds.second.first);
        }
    }
}
