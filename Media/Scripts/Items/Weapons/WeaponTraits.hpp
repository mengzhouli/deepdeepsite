#pragma once

namespace Dwarf
{
    namespace Animation
    {
        class SkeletonInstance;
    }

    namespace Item
    {
        class BuildRateModifier
        {
        public:
            virtual float GetBuildRateMultiplier() const = 0;
        };

        class Miner
        {
        public:
            virtual const Vector2f& GetTipPosition() const = 0;
            virtual float GetMiningRate() const = 0;
        };

        class MinerSack
        {
        };

        class MineRateModifier
        {
        public:
            virtual float GetMineRateMultiplier() const = 0;
        };

        class GrappleAttachable
        {
        public:
            virtual void SetGrappleAttached(bool attached) = 0;
        };

        class FlareAttachable
        {
        public:
            virtual void SetFlareAttached(bool attached) = 0;
        };

        class MuzzleHaver
        {
        public:
            virtual Rayf GetMuzzlePosition() const = 0;
        };

        class ChannelingFoodModifier
        {
        public:
            virtual float GetCookRangeModifier() const = 0;
            virtual uint32_t GetCookFoodAdditionalTargetCount() const = 0;
            virtual bool CookedFoodDoesAOE() const = 0;
            virtual bool CookedFoodTargetsEnemies() const = 0;
            virtual bool CookedFoodHealsOverTime() const = 0;
        };

        class MouseLooker
        {
        public:
            virtual bool WantsToLookAtMouse() const = 0;
        };

        class SkeletonHaver
        {
        public:
            virtual Animation::SkeletonInstance* GetSkeleton() const = 0;
        };

        class LightEmitter
        {
        public:
            virtual void SetLightEnabled(bool lightEnabled) = 0;
            virtual bool IsLightEnabled() const = 0;
        };
    }
}
