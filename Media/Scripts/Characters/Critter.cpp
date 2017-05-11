#include "Characters/Critter.hpp"

namespace Dwarf
{
    namespace Character
    {
        Critter::Critter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicCharacter(parameters, skeletonPath, matsetPath)
            , _followingSpline(false)
            , _followSpline()
            , _currentTargetT(Random::RandomBetween(0.0f, 1.0f))
            , _nextMoveTimer(0.0f)
        {
        }

        void Critter::SetFollowSpline(const Splinef& spline)
        {
            _followingSpline = true;
            _followSpline = spline;
        }

        void Critter::OnStopMoving()
        {
            BasicCharacter::OnStopMoving();

            if (_followingSpline)
            {
                _nextMoveTimer = Random::RandomBetween(3.0f, 10.0f);
            }
        }

        void Critter::OnUpdate(double totalTime, float dt)
        {
            BasicCharacter::OnUpdate(totalTime, dt);

            if (_followingSpline && !IsMoving())
            {
                _nextMoveTimer -= dt;
                if (_nextMoveTimer < 0.0f)
                {
                    int32_t attempts = 10;
                    while (attempts-- > 0)
                    {
                        float minTarget = Max(_currentTargetT - 0.3f, 0.0f);
                        float maxTarget = Min(_currentTargetT + 0.3f, 1.0f);
                        _currentTargetT = Random::RandomBetween(minTarget, maxTarget);
                        Vector2f nextMovePos = _followSpline.Evalulate(_currentTargetT);
                        auto pathPos = GetLevelLayer()->RayCastTerrain(Rayf(nextMovePos, Vector2f::UnitY), GetPathableEdges());
                        if (pathPos)
                        {
                            PushAction(CreateMoveAction(pathPos->GetPosition()), false);
                            break;
                        }
                    }

                    if (attempts < 0)
                    {
                        LogInfo("Critter", "Failed to find any pathable locations on the movement spline");
                    }
                }
            }
        }

        const std::string RatSkeleton = "Skeletons/Characters/Critters/rat.skel";
        const std::vector<std::string> RatMaterials =
        {
            "Skeletons/Characters/Critters/rat1.polymatset",
            "Skeletons/Characters/Critters/rat2.polymatset",
            "Skeletons/Characters/Critters/rat3.polymatset",
        };

        Rat::Rat(const CharacterParameters& parameters)
            : Critter(parameters, RatSkeleton, Random::RandomItem(RatMaterials))
        {
            SetMoveType(MoveType_Walk);
            SetPathableEdges(Pathfinding::EdgeType_Walk);
            SetMoveSpeed(250.0f);
            SetMaxHealth(5);
            SetHealthRegeneration(0.0f);

            SetEntityMask(CharacterMask_Aesthetic);

            SetSkeletonRotatesWithDirection(true);

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run", 1.0f);
        }

        Rat::Rat(const CharacterParameters& parameters, const Splinef& followSpline)
            : Rat(parameters)
        {
            SetFollowSpline(followSpline);
        }

        const std::string SkeletonRatSkeleton = "Skeletons/Characters/Critters/rat.skel";
        const std::vector<std::string> SkeletonRatMaterials =
        {
            "Skeletons/Characters/Critters/rat4.polymatset",
        };

        SkeletonRat::SkeletonRat(const CharacterParameters& parameters)
            : Critter(parameters, SkeletonRatSkeleton, Random::RandomItem(SkeletonRatMaterials))
        {
            SetMoveType(MoveType_Walk);
            SetPathableEdges(Pathfinding::EdgeType_Walk);
            SetMoveSpeed(250.0f);
            SetMaxHealth(5);
            SetHealthRegeneration(0.0f);

            SetEntityMask(CharacterMask_Aesthetic);

            SetSkeletonRotatesWithDirection(true);

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle", 1.0f);
            AddTerrainAnimation(AnimationType_Move, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "run", 1.0f);
        }

        SkeletonRat::SkeletonRat(const CharacterParameters& parameters, const Splinef& followSpline)
            : SkeletonRat(parameters)
        {
            SetFollowSpline(followSpline);
        }
    }

    template <>
    void EnumeratePreloads<Character::Critter>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::Rat>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Critter>(preloads);

        preloads.insert(Character::RatSkeleton);
        preloads.insert(Character::RatMaterials.begin(), Character::RatMaterials.end());
    }

    template <>
    void EnumeratePreloads<Character::SkeletonRat>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Critter>(preloads);

        preloads.insert(Character::SkeletonRatSkeleton);
        preloads.insert(Character::SkeletonRatMaterials.begin(), Character::SkeletonRatMaterials.end());
    }
}
