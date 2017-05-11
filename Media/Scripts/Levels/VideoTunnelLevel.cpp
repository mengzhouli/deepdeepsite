#include "Levels/VideoTunnelLevel.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Armors/BrewerArmor.hpp"
#include "Items/Trinkets/Pipe.hpp"

#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Items/Weapons/Hammer.hpp"
#include "Items/Armors/BuilderArmor.hpp"

#include "Characters/Dwarves/CookDwarf.hpp"
#include "Items/Weapons/Kitchenware.hpp"
#include "Items/Armors/CookArmor.hpp"
#include "Items/Trinkets/Cookbook.hpp"

#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Items/Weapons/Sword.hpp"
#include "Items/Weapons/Shield.hpp"
#include "Items/Armors/FighterArmor.hpp"

#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Items/Weapons/PickAxe.hpp"
#include "Items/Weapons/Sack.hpp"
#include "Items/Armors/MinerArmor.hpp"

#include "Characters/Dwarves/NavigatorDwarf.hpp"
#include "Items/Weapons/Gun.hpp"
#include "Items/Armors/NavigatorArmor.hpp"
#include "Items/Trinkets/Headlamp.hpp"

namespace Dwarf
{
    namespace Level
    {
        VideoTunnelLevel::VideoTunnelLevel(const LevelParameters& parameters)
            : BasicLevel(parameters)
        {
        }

        void VideoTunnelLevel::OnCreate()
        {
            BasicLevel::OnCreate();

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();
            primaryLayer->SetKillOnLeavingCameraBounds(false);

            Pathfinding::PathSystem* pathSystem = primaryLayer->GetPathSystem();
            pathSystem->AddChain(Chainf(primaryLayer->GetSpline("rope_spline").Points()), Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope);
        }

        void VideoTunnelLevel::OnUpdate(double totalTime, float dt)
        {
            BasicLevel::OnUpdate(totalTime, dt);

            auto& cameraController = GetCameraController();
            const auto& cameraBounds = cameraController.GetBounds().Bounds();

            cameraController.Pan(cameraBounds.Middle());
            SetTargetCameraViewSize(cameraBounds.Size);

            LevelLayerInstance* primaryLayer = GetPrimaryLayer();

            if (_dwarves.empty())
            {
                Pathfinding::PathSystem* pathSystem = primaryLayer->GetPathSystem();

                _pathStart = primaryLayer->GetTriggerPosition("path_start");
                _pathEnd = primaryLayer->GetTriggerPosition("path_end");
                auto path = pathSystem->ComputePath(_pathStart, Pathfinding::EdgeType_All,
                                                    _pathEnd, Pathfinding::EdgeType_All,
                                                    500.0f, Pathfinding::EdgeType_All, false);
                assert(path);

                float pathLength = path->GetLength();
                const uint32_t dwarfCount = 6;
                float dwarfSeparation = pathLength / dwarfCount;

                static const Color minColor = Color::FromFloats(0.2f, 0.2f, 0.2f, 1.0f);

                Vector2f spawnOffset(20.0f, 0.0f);

                {
                    Character::NavigatorDwarf* navigator = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "navigator", nullptr, Character::BindCharacterConstructor<Character::NavigatorDwarf>());
                    navigator->SetMinimumColor(minColor);
                    navigator->GiveItem(Item::BindItemConstructor<Item::BasicPistol>());
                    navigator->GiveItem(Item::BindItemConstructor<Item::NavigatorTunic>());
                    navigator->GiveItem(Item::BindItemConstructor<Item::BasicHeadlamp>());
                    navigator->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(navigator->GetID());

                    path->MoveForward(dwarfSeparation);
                }

                {
                    Character::FighterDwarf* fighter = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "fighter", nullptr, Character::BindCharacterConstructor<Character::FighterDwarf>());
                    fighter->SetMinimumColor(minColor);
                    fighter->GiveItem(Item::BindItemConstructor<Item::IronShortSword>());
                    fighter->GiveItem(Item::BindItemConstructor<Item::IronHeater>());
                    fighter->GiveItem(Item::BindItemConstructor<Item::Chainmail>());
                    fighter->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(fighter->GetID());

                    path->MoveForward(dwarfSeparation);
                }

                {
                    Character::CookDwarf* cook = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "cook", nullptr, Character::BindCharacterConstructor<Character::CookDwarf>());
                    cook->SetMinimumColor(minColor);
                    cook->GiveItem(Item::BindItemConstructor<Item::Cutlery>());
                    cook->GiveItem(Item::BindItemConstructor<Item::DeVermisCoctioneCookbook>());
                    cook->GiveItem(Item::BindItemConstructor<Item::Apron>());
                    cook->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(cook->GetID());

                    path->MoveForward(dwarfSeparation);
                }

                {
                    Character::BrewerDwarf* brewer = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "brewer", nullptr, Character::BindCharacterConstructor<Character::BrewerDwarf>());
                    brewer->SetMinimumColor(minColor);
                    brewer->GiveItem(Item::BindItemConstructor<Item::BasicRifle>());
                    brewer->GiveItem(Item::BindItemConstructor<Item::BriarPipe>());
                    brewer->GiveItem(Item::BindItemConstructor<Item::BrewerTunic>());
                    brewer->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(brewer->GetID());

                    path->MoveForward(dwarfSeparation);
                }

                {
                    Character::MinerDwarf* miner = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "miner", nullptr, Character::BindCharacterConstructor<Character::MinerDwarf>());
                    miner->SetMinimumColor(minColor);
                    miner->GiveItem(Item::BindItemConstructor<Item::IronPickAxe>());
                    miner->GiveItem(Item::BindItemConstructor<Item::Sack>());
                    miner->GiveItem(Item::BindItemConstructor<Item::MinerTunic>());
                    miner->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(miner->GetID());

                    path->MoveForward(dwarfSeparation);
                }

                {
                    Character::BuilderDwarf* builder = primaryLayer->SpawnCharacter(path->GetCurrentPosition()->GetPosition() + spawnOffset, "builder", nullptr, Character::BindCharacterConstructor<Character::BuilderDwarf>());
                    builder->SetMinimumColor(minColor);
                    builder->GiveItem(Item::BindItemConstructor<Item::IronHammer>());
                    builder->GiveItem(Item::BindItemConstructor<Item::BrewerTunic>());
                    builder->PushAction(Character::CreateMoveAction(_pathEnd), false);
                    _dwarves.push_back(builder->GetID());

                    path->MoveForward(dwarfSeparation);
                }
            }
            else if (_frame > 1)
            {
                bool firstDwarf = true;
                for (auto dwarfID : _dwarves)
                {
                    auto dwarf = primaryLayer->GetCharacter(dwarfID);
                    assert(dwarf);

                    if (!dwarf->IsMoving())
                    {
                        dwarf->SetPosition(_pathStart);
                        dwarf->PushAction(Character::CreateMoveAction(_pathEnd), false);

                        if (firstDwarf)
                        {
                            LogInfo(Format("Loop took %u frames, %g seconds.", _loopCounter, _loopTime));
                            _loopCounter = 0;
                            _loopTime = 0;
                        }
                    }

                    firstDwarf = false;
                }
            }

            _loopCounter++;
            _loopTime += dt;
            _frame++;
        }

        void VideoTunnelLevel::OnDraw(LevelLayerInstance* layer, Graphics::LevelRenderer* levelRenderer) const
        {
            BasicLevel::OnDraw(layer, levelRenderer);
        }
    }

    template <>
    void EnumeratePreloads<Level::VideoTunnelLevel>(PreloadSet& preloads)
    {
        EnumeratePreloads<Level::BasicLevel>(preloads);
    }
}
