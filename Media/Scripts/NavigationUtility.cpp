#include "NavigationUtility.hpp"

#include "Characters/GrappleRope.hpp"
#include "Characters/Ladder.hpp"

namespace Dwarf
{
    namespace Level
    {
        static bool IsValidResult(std::shared_ptr<Pathfinding::PathPosition> result, const Character::Character* character, const Vector2f& destination, float maxSearchDist)
        {
            return result != nullptr &&
                Vector2f::Distance(destination, result->GetPosition()) <= maxSearchDist && result->IsWalkable();
        }

        bool GetMoveTarget(const LevelLayerInstance* layer, const Character::Character* character, uint32_t invalidEdgeTypes, const Vector2f& destination, float maxSearchDist,
            Vector2f& outMoveTarget, Vector2f& outMoveNormal)
        {
            Pathfinding::EdgeType edgeType = static_cast<Pathfinding::EdgeType>(character->GetPathableEdges() & ~invalidEdgeTypes);

            const float nonVerticalSearchDistMult = 0.15f;
            std::shared_ptr<Pathfinding::PathPosition> searchResult = layer->FindPathEdge(destination, maxSearchDist * nonVerticalSearchDistMult, edgeType);
            if (IsValidResult(searchResult, character, destination, maxSearchDist))
            {
                outMoveTarget = searchResult->GetPosition();
                outMoveNormal = searchResult->GetNormal();
                return true;
            }

            const uint32_t numRays = 10;
            for (uint32_t i = 0; i < numRays; i++)
            {
                Rayf downRay(destination - (Vector2f::UnitY * (i * (maxSearchDist / numRays))), Vector2f::UnitY);
                std::shared_ptr<Pathfinding::PathPosition> rayResult = layer->RayCastTerrain(downRay, edgeType);

                if (IsValidResult(rayResult, character, destination, maxSearchDist))
                {
                    outMoveTarget = rayResult->GetPosition();
                    outMoveNormal = rayResult->GetNormal();
                    return true;
                }
            }

            return false;
        }

        bool GetPlaceTarget(const LevelLayerInstance* layer, Pathfinding::EdgeType edgeTypes, const Vector2f& destination,
            float maxSearchDist, Vector2f& outPlaceTarget)
        {
            Pathfinding::EdgeType cleanedEdgeTypes =
                static_cast<Pathfinding::EdgeType>(edgeTypes & ~Pathfinding::EdgeType_Climb & ~Pathfinding::EdgeType_Jump);

            const uint32_t numRays = 10;
            for (uint32_t i = 0; i < numRays; i++)
            {
                Rayf downRay(destination - (Vector2f::UnitY * (i * (maxSearchDist / numRays))), Vector2f::UnitY);
                std::shared_ptr<Pathfinding::PathPosition> result = layer->RayCastTerrain(downRay, cleanedEdgeTypes);

                if (result != nullptr && Vector2f::Distance(destination, result->GetPosition()) <= maxSearchDist)
                {
                    outPlaceTarget = result->GetPosition();
                    return true;
                }
            }

            return false;
        }

        Character::CharacterConstructor<Character::GrappleRope> BindGrappleConstructor(LevelLayerInstance* layer, const Splinef& location)
        {
            assert(layer != nullptr && location.Size() >= 2);

            Vector2f top, bottom;
            if (location.At(0).Y > location.At(location.Size() - 1).Y)
            {
                top = location.At(location.Size() - 1);
                bottom = location.At(0);
            }
            else
            {
                top = location.At(0);
                bottom = location.At(location.Size() - 1);
            }

            std::shared_ptr<Pathfinding::PathPosition> a = layer->FindLedge(top, 500.0f, Pathfinding::EdgeType_Walk);
            std::shared_ptr<Pathfinding::PathPosition> b = layer->FindPathEdge(bottom, 500.0f, Pathfinding::EdgeType_Walk);

            //assert(a != nullptr && b != nullptr);

            Vector2f aPos = top;
            if (a != nullptr)
            {
                aPos = a->GetPosition();
            }

            Vector2f bPos = bottom;
            if (b != nullptr)
            {
                bPos = b->GetPosition();
            }

            return Character::BindCharacterConstructor<Character::GrappleRope>(bPos, bPos, aPos, 0.0f);
        }

        Character::CharacterConstructor<Character::Ladder> BindLadderConstructor(LevelLayerInstance* layer, const Splinef& location)
        {
            assert(layer != nullptr && location.Size() >= 2);

            Vector2f top, bottom;
            if (location.At(0).Y > location.At(location.Size() - 1).Y)
            {
                top = location.At(location.Size() - 1);
                bottom = location.At(0);
            }
            else
            {
                top = location.At(0);
                bottom = location.At(location.Size() - 1);
            }

            std::shared_ptr<Pathfinding::PathPosition> a = layer->FindLedge(top, 500.0f, Pathfinding::EdgeType_Walk);
            std::shared_ptr<Pathfinding::PathPosition> b = layer->FindPathEdge(bottom, 500.0f, Pathfinding::EdgeType_Walk);

            //assert(a != nullptr && b != nullptr);

            Vector2f aPos = top;
            if (a != nullptr)
            {
                aPos = a->GetPosition();
            }

            Vector2f bPos = bottom;
            if (b != nullptr)
            {
                bPos = b->GetPosition();
            }

            return Character::BindCharacterConstructor<Character::Ladder>(aPos, bPos, a != nullptr && b != nullptr);
        }
    }
}
