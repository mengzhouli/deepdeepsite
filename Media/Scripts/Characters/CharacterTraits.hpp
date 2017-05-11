#pragma once

#include "Ray.hpp"
#include "Pathfinding/PathEdge.hpp"

#include <string>

namespace Dwarf
{
    namespace Character
    {
        class Character;

        class Thrower
        {
        public:
            virtual void PlayThrowAnimation() = 0;
            virtual std::string GetThrownItemAttachPointName() const = 0;
            virtual bool HasThrowReleasePassed() const = 0;
            virtual bool HasThrowAnimationFinished() const = 0;
        };

        class Eater
        {
        public:
            virtual void OnPostEat(Character* food) { };
        };

        class Drinker
        {
        public:
            virtual void OnPostDrink(Character* drink) { };
        };

        class Edible
        {
        private:
            virtual void MakeMePolymorphic() { };
        };

        BETTER_ENUM(DrinkableType, uint8_t, Beer, Wine, Unknown);
        class Drinkable
        {
        public:
            virtual DrinkableType GetDrinkableType() const = 0;
        };

        class Reviveable
        {
        public:
            virtual bool IsDowned() const = 0;
            virtual float Revive(Character* reviver) = 0;
        };

        class Builder
        {
        public:
            virtual void PlayBuildAnimation() = 0;
        };

        class Leaper
        {
        public:
            virtual float OnPreLeap(Pathfinding::TerrainType terrainType) = 0; // Return the time needed to wind up
            virtual void OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime) = 0;
            virtual float OnPostLeap(Pathfinding::TerrainType terrainType) { return 0.0f; };
        };

        class GroundSlammer
        {
        public:
            struct Info
            {
                float TotalDuration;
                float HitTime;
            };

            virtual Info OnGroundSlam(Pathfinding::TerrainType terrainType) = 0;
            virtual void OnGroundSlamLand(Pathfinding::TerrainType terrainType) { };
        };

        class Smoker
        {
        public:
            virtual std::string GetPipeAttachPointName() const = 0;
        };

        class Cooker
        {
        public:
            virtual void PlayCookAnimation() = 0;
            virtual std::string GetCookbookAttachPointName() const = 0;
            virtual std::string GetCookPotAttachPointName() const = 0;

            virtual void PlayFoodThrowAnimation() = 0;
            virtual std::string GetFoodThrownItemAttachPointName() const = 0;
            virtual bool HasFoodThrowReleasePassed() const = 0;
            virtual bool HasFoodThrowAnimationFinished() const = 0;
        };

        class LampHolder
        {
        public:
            virtual std::string GetLampAttachPoint() const = 0;
        };

        class Brewer
        {
        public:
            struct Info
            {
                float TotalDuration;
                float KegSpawnTime;
                float KegDropTime;
            };
            virtual Info PlayKegThrowdownAnimation() = 0;
            virtual std::string GetKegAttachPointName() const = 0;
        };

        class SackSmasher
        {
        public:
            struct Info
            {
                float TotalDuration;
                float WooshTime;
                float SackHitTime;
            };
            virtual Info PlaySackSmashAnimation() = 0;
        };
    }
}
