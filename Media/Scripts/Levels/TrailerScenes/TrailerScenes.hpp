#pragma once

#include "GameState/LevelGameState.hpp"

namespace Dwarf
{
    namespace Level
    {
        GameState::LevelConstructor<> CreateTowerStatueApproachScene();
        GameState::LevelConstructor<> CreateTowerGrappleScene();
        GameState::LevelConstructor<> CreateBridgeBattleScene();
        GameState::LevelConstructor<> CreateChestApproachScene();
        GameState::LevelConstructor<> CreateDynamiteScene();
        GameState::LevelConstructor<> CreateShieldBossIntroScene();
        GameState::LevelConstructor<> CreateKitchenLookaroundScene();
        GameState::LevelConstructor<> CreateTitleLightScene();
        GameState::LevelConstructor<> CreateEndToEndFlythroughScene();
    }
}
