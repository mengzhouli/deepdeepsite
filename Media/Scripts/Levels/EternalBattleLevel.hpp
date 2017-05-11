#pragma once

#include "Levels/BasicLevel.hpp"

#include "Controllers/Player.hpp"
#include "Controllers/WildlifeController.hpp"

#include "Characters/MrBones.hpp"
#include "Characters/Torch.hpp"
#include "Characters/ResourceNode.hpp"
#include "Characters/Bridge.hpp"

#include "Items/Weapons/SkeletonWeapons.hpp"

#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/LightningSword.hpp"
#include "Items/Weapons/Gun.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"

#include "Items/Weapons/Hammer.hpp"

#include "Items/Weapons/PickAxe.hpp"


namespace Dwarf
{
    namespace Level
    {
        class EternalBattleLevel : public BasicLevel
        {
        public:
            EternalBattleLevel(const LevelParameters& parameters)
                : BasicLevel(parameters)
                , _spawnTimer(0.0f)
                , _minSpawnInterval(0.65f)
                , _maxBottomDwarves(6)
                , _maxTopDwarves(1)
                , _maxBottomSkellies(13)
                , _maxTopSkellies(1)
            {
                _bottomDwarfWeapons.push_back(Item::BindItemConstructor<Item::IronShortSword>());

                _topDwarfWeapons.push_back(Item::BindItemConstructor<Item::BasicRifle>());

                _bottomSkellyWeapons.push_back(Item::BindItemConstructor<Item::SkelAxe>());
                _bottomSkellyWeapons.push_back(Item::BindItemConstructor<Item::SkelDagger>());
                _bottomSkellyWeapons.push_back(Item::BindItemConstructor<Item::SkelHalberd>());
                _bottomSkellyWeapons.push_back(Item::BindItemConstructor<Item::SkelScimitar>());

                _topSkellyWeapons.push_back(Item::BindItemConstructor<Item::SkelBow>());
            }

            virtual void OnCreate() override
            {
                _dwarfController = CreateController("dwarf_controller", Character::BindControllerConstructor<Character::WildlifeController>());
                _skellyController = CreateController("skeleton_controller", Character::BindControllerConstructor<Character::WildlifeController>());

                initializeCamera();
                spawnBridge();
                spawnTorches();
            }

        protected:
            virtual ~EternalBattleLevel()
            {
            }

            void OnUpdate(double totalTime, float dt)
            {
                BasicLevel::OnUpdate(totalTime, dt);

                cleanCharacterList(_bottomDwarves);
                cleanCharacterList(_topDwarves);
                cleanCharacterList(_bottomSkellies);
                cleanCharacterList(_topSkellies);

                _spawnTimer += dt;
                if (_spawnTimer > _minSpawnInterval)
                {
                    float bottomDwarfPerc = _bottomDwarves.size() / float(_maxBottomDwarves);
                    float topDwarfPerc = _topDwarves.size() / float(_maxTopDwarves);
                    float bottomSkellyPerc = _bottomSkellies.size() / float(_maxBottomSkellies);
                    float topSkellyPerc = _topSkellies.size() / float(_maxTopSkellies);

                    if (bottomDwarfPerc <= topDwarfPerc && bottomDwarfPerc <= bottomSkellyPerc && bottomDwarfPerc <= topSkellyPerc && _bottomDwarves.size() < _maxBottomDwarves)
                    {
                        spawnBottomDwarf();
                    }
                    else if (topDwarfPerc <= bottomSkellyPerc && topDwarfPerc <= topSkellyPerc && _topDwarves.size() < _maxTopDwarves)
                    {
                        spawnTopDwarf();
                    }
                    else if (bottomSkellyPerc <= topSkellyPerc && _bottomSkellies.size() < _maxBottomSkellies)
                    {
                        spawnBottomSkelly();
                    }
                    else if (_topSkellies.size() < _maxTopSkellies)
                    {
                        spawnTopSkelly();
                    }

                    _spawnTimer = 0.0f;
                }
            }

        private:
            void initializeCamera()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();
                CameraController& cameraController = GetCameraController();

                const Vector2f& cameraPos = primaryLayer->GetTriggerPosition("camera_pos");
                const Vector2f& cameraRight = primaryLayer->GetTriggerPosition("camera_right");

                cameraController.Pan(cameraPos);

                const RotatedRectanglef& curBounds = cameraController.GetCamera().GetViewBounds();

                float curRadius = curBounds.ExtentX;
                float targetRadius = cameraRight.X - cameraPos.X;

                float curZoom = cameraController.GetCamera().GetZoom();
                float targetZoom = 1.0f / ((curZoom / curRadius) * targetRadius);

                cameraController.Zoom(targetZoom);
            }

            void spawnBridge()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                std::shared_ptr<Pathfinding::PathPosition> leftBridgePos = primaryLayer->FindLedge(primaryLayer->GetTriggerPosition("bridge_left"), 100.0f, Pathfinding::EdgeType_Walk);
                assert(leftBridgePos);

                std::shared_ptr<Pathfinding::PathPosition> rightBridgePos = primaryLayer->FindLedge(primaryLayer->GetTriggerPosition("bridge_right"), 100.0f, Pathfinding::EdgeType_Walk);
                assert(rightBridgePos);

                primaryLayer->SpawnCharacter(leftBridgePos->GetPosition(), "bridge", NULL,
                                             Character::BindCharacterConstructor<Character::Bridge>(leftBridgePos->GetPosition(),
                                                                                                    rightBridgePos->GetPosition(), 0.0f));
            }

            void spawnTorches()
            {
                SpawnCharactersAtTriggers("torch_spawn", Character::BindCharacterConstructor<Character::Torch>(BaseFireColor, false, true, true), "torch", NULL);
            }

            void spawnBottomDwarf()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                const Vector2f& spawnPos = primaryLayer->GetTriggerPosition("spawn_left_bottom");
                const Vector2f& attackPos = primaryLayer->GetTriggerPosition("attack_left_bottom");

                Character::Dwarf* dwarf = primaryLayer->SpawnCharacter(spawnPos, "bottom_dwarf", _dwarfController, Character::BindCharacterConstructor<Character::FighterDwarf>());
                dwarf->GiveItem(Random::RandomItem(_bottomDwarfWeapons));
                dwarf->PushAction(Character::CreateAttackMoveAction(attackPos), false);

                _bottomDwarves.insert(dwarf->GetID());
            }

            void spawnTopDwarf()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                const Vector2f& spawnPos = primaryLayer->GetTriggerPosition("spawn_left_top");
                const Vector2f& attackPos = primaryLayer->GetTriggerPosition("attack_left_top");

                Character::Dwarf* dwarf = primaryLayer->SpawnCharacter(spawnPos, "top_dwarf", _dwarfController, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                dwarf->GiveItem(Random::RandomItem(_topDwarfWeapons));
                dwarf->PushAction(Character::CreateAttackMoveAction(attackPos), false);
                dwarf->PushAction(Character::CreateHoldPositionAction(), true);

                _topDwarves.insert(dwarf->GetID());
            }

            void spawnBottomSkelly()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                const Vector2f& spawnPos = primaryLayer->GetTriggerPosition("spawn_right_bottom");
                const Vector2f& attackPos = primaryLayer->GetTriggerPosition("attack_right_bottom");

                Character::MrBones* skelly = primaryLayer->SpawnCharacter(spawnPos, "bottom_skelly", _skellyController, Character::BindCharacterConstructor<Character::MrBonesMelee>());
                skelly->GiveItem(Random::RandomItem(_bottomSkellyWeapons));
                skelly->PushAction(Character::CreateAttackMoveAction(attackPos), false);

                _bottomSkellies.insert(skelly->GetID());
            }

            void spawnTopSkelly()
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                const Vector2f& spawnPos = primaryLayer->GetTriggerPosition("spawn_right_top");
                const Vector2f& attackPos = primaryLayer->GetTriggerPosition("attack_right_top");

                Character::MrBones* skelly = primaryLayer->SpawnCharacter(spawnPos, "top_skelly", _skellyController, Character::BindCharacterConstructor<Character::MrBonesArcher>());
                skelly->GiveItem(Random::RandomItem(_topSkellyWeapons));
                skelly->PushAction(Character::CreateAttackMoveAction(attackPos), false);
                skelly->PushAction(Character::CreateHoldPositionAction(), true);

                _topSkellies.insert(skelly->GetID());
            }

            void cleanCharacterList(std::set<Character::CharacterID>& characters)
            {
                LevelLayerInstance* primaryLayer = GetPrimaryLayer();

                std::set<Character::CharacterID>::iterator i = characters.begin();
                while (i != characters.end())
                {
                    Character::Character* character = primaryLayer->GetCharacter<Character::Character>(*i);
                    if (!character || !character->IsAlive())
                    {
                        i = characters.erase(i);
                    }
                    else
                    {
                        i++;
                    }
                }
            }

            float _spawnTimer;
            const float _minSpawnInterval;

            const uint32_t _maxBottomDwarves;
            std::set<Character::CharacterID> _bottomDwarves;
            std::vector< Item::ItemConstructor<Item::Weapon> > _bottomDwarfWeapons;

            const uint32_t _maxTopDwarves;
            std::set<Character::CharacterID> _topDwarves;
            std::vector< Item::ItemConstructor<Item::Weapon> > _topDwarfWeapons;

            const uint32_t _maxBottomSkellies;
            std::set<Character::CharacterID> _bottomSkellies;
            std::vector< Item::ItemConstructor<Item::Weapon> > _bottomSkellyWeapons;

            const uint32_t _maxTopSkellies;
            std::set<Character::CharacterID> _topSkellies;
            std::vector< Item::ItemConstructor<Item::Weapon> > _topSkellyWeapons;

            Character::WildlifeController* _dwarfController;
            Character::WildlifeController* _skellyController;
        };
    }

    template <>
    void EnumeratePreloads<Level::EternalBattleLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);

        EnumeratePreloads<Character::MrBonesMelee>(preloads);
        EnumeratePreloads<Character::MrBonesArcher>(preloads);

        EnumeratePreloads<Item::SkelAxe>(preloads);
        EnumeratePreloads<Item::SkelDagger>(preloads);
        EnumeratePreloads<Item::SkelHalberd>(preloads);
        EnumeratePreloads<Item::SkelScimitar>(preloads);
        EnumeratePreloads<Item::SkelBow>(preloads);

        EnumeratePreloads<Character::Bridge>(preloads);

        EnumeratePreloads<Character::Torch>(preloads);

        EnumeratePreloads<Character::BrewerDwarf>(preloads);
        EnumeratePreloads<Character::BuilderDwarf>(preloads);
        EnumeratePreloads<Character::CookDwarf>(preloads);
        EnumeratePreloads<Character::FighterDwarf>(preloads);
        EnumeratePreloads<Character::MinerDwarf>(preloads);
        EnumeratePreloads<Character::NavigatorDwarf>(preloads);

        EnumeratePreloads<Item::IronShortSword>(preloads);
        EnumeratePreloads<Item::BasicRifle>(preloads);
        EnumeratePreloads<Item::LightningSword>(preloads);

        EnumeratePreloads<Item::IronHammer>(preloads);

        EnumeratePreloads<Item::IronPickAxe>(preloads);
    }
}
