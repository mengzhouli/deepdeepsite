#pragma once

#include "Level/LevelLayerInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        class GrappleRope;
        class Ladder;
    }

    namespace Level
    {
        bool GetMoveTarget(const LevelLayerInstance* layer, const Character::Character* character, uint32_t invalidEdgeTypes, const Vector2f& destination, float maxSearchDist,
                           Vector2f& outMoveTarget, Vector2f& outMoveNormal);

        bool GetPlaceTarget(const LevelLayerInstance* layer, Pathfinding::EdgeType edgeTypes, const Vector2f& destination, float maxSearchDist,
                            Vector2f& outPlaceTarget);

        Character::CharacterConstructor<Character::GrappleRope> BindGrappleConstructor(LevelLayerInstance* layer, const Splinef& location);
        Character::CharacterConstructor<Character::Ladder> BindLadderConstructor(LevelLayerInstance* layer, const Splinef& location);
    }
}
