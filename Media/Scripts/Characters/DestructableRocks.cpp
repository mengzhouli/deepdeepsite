#include "Characters/DestructableRocks.hpp"

#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const float FadeStartTime = 10.0f;
        static const float FadeDuration = 4.0f;

        DestructableRocks::DestructableRocks(const CharacterParameters& parameters, const std::string& collisionName, const std::string& skeletonPath,
                                             const std::string& materialSetPath, OnDestructionCallback callback)
            : SkeletonCharacter(parameters, skeletonPath, materialSetPath)
            , _collisionName(collisionName)
            , _lastDmg()
            , _lastDmgPos()
            , _timeDead(0.0f)
            , _destroyed(false)
            , _callback(callback)
        {
            SetMaxHealth(10.0f);

            SetEntityMask(CharacterMask_Destructable);
            SetMoveType(MoveType_None);

            DisablePhysics();

            SetSkeletonCastsShadows(true);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);
            SetSkeletonJointStrength(0.0f);

            SetAttachToGroundOnSpawn(false);
        }

        void DestructableRocks::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            if (_destroyed)
            {
                _timeDead += dt;

                if (_timeDead > FadeStartTime)
                {
                    uint8_t alpha = Lerp(0, 255, 1.0f - Clamp((_timeDead - FadeStartTime) / FadeDuration, 0.0f, 1.0f));

                    Color color = GetColor();
                    color.A = alpha;
                    SetColor(color);
                }

                if (_timeDead > FadeStartTime + FadeDuration)
                {
                    Terminate();
                }
            }
        }

        Damage DestructableRocks::OnPreRecieveDamage(Character* source, const Vector2f& pos, const Damage& dmg)
        {
            Damage updatedDmg = SkeletonCharacter::OnPreRecieveDamage(source, pos, dmg);

            if ((dmg.Type & DamageType_Type_Explosion) != 0)
            {
                SetSkeletonJointStrength(0.0f);
                EnablePhysics(true);
                ApplyRadialImpulse(GetBounds().Middle(), 1500.0f * GetCollision()->GetMass());
                GetLevelLayer()->SetTerrainCollisionEnabled(_collisionName, false);
                SetSkeletonCastsShadows(false);
                _destroyed = true;
                if (_callback)
                {
                    _callback();
                }
            }

            updatedDmg.Amount = 0;
            return updatedDmg;
        }

        static const std::string DestructableRubbleSkeletonPath = "Skeletons/Characters/Rubble/rubble0.skel";
        static const std::map<RubbleSize, std::string> RubbleMaterialPaths =
        {
            { RubbleSize_Small, "Skeletons/Characters/Rubble/rubble_small.polymatset" },
            { RubbleSize_Medium, "Skeletons/Characters/Rubble/rubble_medium.polymatset" },
            { RubbleSize_Large, "Skeletons/Characters/Rubble/rubble_large.polymatset" },
        };

        DestructableRubble::DestructableRubble(const CharacterParameters& parameters, RubbleSize size, const std::string& collisionName,
                                               OnDestructionCallback callback)
            : DestructableRocks(parameters, collisionName, DestructableRubbleSkeletonPath, RubbleMaterialPaths.at(size), callback)
        {
            SetTooltip("character_destructable_rocks_name");
        }

        static const std::string DestructableWallSkeletonPath = "Skeletons/Characters/D_wall/breakable_wall.skel";
        static const std::string DestructableWallMaterialPaths = "Skeletons/Characters/D_wall/breakable_wall.polymatset";

        DestructableWall::DestructableWall(const CharacterParameters& parameters, const std::string& collisionName, OnDestructionCallback callback)
            : DestructableRocks(parameters, collisionName, DestructableWallSkeletonPath, DestructableWallMaterialPaths, callback)
        {
            SetTooltip("character_destructable_wall_name");
        }
    }

    template <>
    void EnumeratePreloads<Character::DestructableRocks>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::DestructableRubble>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::DestructableRocks>(preloads);
        preloads.insert(Character::DestructableRubbleSkeletonPath);
        for (auto rubbleMatset : Character::RubbleMaterialPaths)
        {
            preloads.insert(rubbleMatset.second);
        }
    }

    template <>
    void EnumeratePreloads<Character::DestructableWall>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::DestructableRocks>(preloads);
        preloads.insert(Character::DestructableWallSkeletonPath);
        preloads.insert(Character::DestructableWallMaterialPaths);
    }
}
