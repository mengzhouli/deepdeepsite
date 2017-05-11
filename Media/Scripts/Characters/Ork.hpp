#pragma once

#include "Characters/BasicCharacter.hpp"
#include "Characters/CharacterTraits.hpp"
#include "TypedEnums.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Ork : public BasicCharacter
        {
        public:
            Ork(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath);

        protected:
            void OnDeath() override;
        };

        class OrkWarrior : public Ork, public Leaper, public GroundSlammer
        {
        public:
            OrkWarrior(const CharacterParameters& parameters);

            // Leaper interface
            float OnPreLeap(Pathfinding::TerrainType terrainType) override;
            void OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime) override;
            float OnPostLeap(Pathfinding::TerrainType terrainType) override;

            // GroundSlammer interface
            GroundSlammer::Info OnGroundSlam(Pathfinding::TerrainType terrainType) override;
            void OnGroundSlamLand(Pathfinding::TerrainType terrainType) override;

        protected:
            void OnUpdate(double totalTime, float dt) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

        private:
            Animation::AnimationSet _leapAnimations;
            Animation::AnimationSet _groundSlamAnimations;
            Audio::SoundSet _leapSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapTerrainSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapLandTerrainSounds;
            Audio::SoundSet _leapLandAlwaysSounds;

            bool _hasSlam;
            float _slamTimer;
        };

        class ShieldOrk : public Ork
        {
        public:
            ShieldOrk(const CharacterParameters& parameters);

            bool HasLineOfSight(const Vector2f& target) const override;

            void StandUp();

        protected:
            void OnSpawn() override;
            void OnDeath() override;

            void OnActionChange(const Action& newAction) override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

            void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

        private:
            enum ShieldState
            {
                Hiding,
                ShieldDown,
                ShieldUp,
            };

            void setShieldState(ShieldState state);
            void onPostShieldSwitch(ShieldState state);

            float _transitionTimer = 0.0f;

            ShieldState _shieldState;

            Animation::AnimationSet _shieldUpAnimations;
            Animation::AnimationSet _shieldDownAnimations;
            Animation::AnimationSet _aggroAnimations;

            std::vector<float> _shieldBreakThresholds;

            Animation::SkeletonInstance* _rubble = nullptr;
            Physics::SkeletonCollision* _rubbleCollision = nullptr;
        };

        class OrkCannoneer : public Ork
        {
        public:
            OrkCannoneer(const CharacterParameters& parameters);

        protected:
            void OnSpawn() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

        private:
            bool _cannonballAttached;
            Animation::SkeletonInstance* _cannonball;
        };

        BETTER_ENUM(OrkLordPhase, uint8_t, ThrowingGoblins, TransitionToBoxing, Boxing);

        class OrkLord : public Ork, public Leaper
        {
        public:
            OrkLord(const CharacterParameters parameters);
            OrkLord(const CharacterParameters parameters, OrkLordPhase phase);

            // Leaper interface
            float OnPreLeap(Pathfinding::TerrainType terrainType) override;
            void OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime) override;
            float OnPostLeap(Pathfinding::TerrainType terrainType) override;

            Rayf GetAttachPoint(const std::string& name) const override;

        protected:
            void OnSpawn() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

        private:
            void setPhase(OrkLordPhase phase);

            void setPhaseAnimations(OrkLordPhase phase);
            void setPhaseWeapons(OrkLordPhase phase, bool justSpawned);
            void spawnCageGobbos();

            Animation::AnimationSet _leapWindUpAnimations;
            Animation::AnimationSet _leapAnimations;
            Animation::AnimationSet _leapLandAnimations;
            Audio::SoundSet _leapSounds;
            Audio::SoundSet _leapLandAlwaysSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapTerrainSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapLandTerrainSounds;

            std::vector<bool> _brokenMats;

            Animation::SkeletonInstance* _gobCage = nullptr;
            Physics::SkeletonCollision* _gobCageCollision = nullptr;

            OrkLordPhase _phase;
        };
    }

    template <>
    void EnumeratePreloads<Character::Ork>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::OrkWarrior>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::ShieldOrk>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::OrkCannoneer>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::OrkLord>(PreloadSet& preloads);
}
