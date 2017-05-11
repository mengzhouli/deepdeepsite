#pragma once

#include "Characters/BasicCharacter.hpp"
#include "Characters/CharacterTraits.hpp"

namespace Dwarf
{
    namespace Character
    {
        struct DwarfInfo
        {
            std::string Name;
            float Size;

            uint32_t Kills;
            float DamageInflicted;
            float DamageRecieved;

            DwarfInfo();
        };

        void WriteToXML(XML::XMLNode& node, const DwarfInfo& value);
        void ReadFromXML(const XML::XMLNode& node, DwarfInfo& value);

        class Dwarf;

        template <typename T = Dwarf>
        using DwarfConstructor = std::function<CharacterConstructor<T>(const DwarfInfo&)>;

        template <typename T, typename... argsT>
        DwarfConstructor<T> BindDwarfConstructor(argsT... args);

        class Dwarf : public BasicCharacter, public Thrower, public Eater, public Leaper, public Smoker, public Cooker, public Brewer, public LampHolder, public Reviveable
        {
        public:
            Dwarf(const CharacterParameters& parameters, const DwarfInfo& info, const std::string& materialSet);

            const DwarfInfo& GetInfo() const;

            const App::Cursor* GetInteractCursor() const override;

            // Thrower interface
            virtual void PlayThrowAnimation() override;
            virtual std::string GetThrownItemAttachPointName() const override;
            virtual bool HasThrowReleasePassed() const override;
            virtual bool HasThrowAnimationFinished() const override;

            // Eater interface
            virtual void OnPostEat(Character* food) override;

            // Leaper interface
            float OnPreLeap(Pathfinding::TerrainType terrainType) override;
            void OnLeap(Pathfinding::TerrainType terrainType, float airTime, float maxAirTime) override;
            float OnPostLeap(Pathfinding::TerrainType terrainType) override;

            // Smoker interface
            virtual std::string GetPipeAttachPointName() const override;

            // Cooker interface
            virtual void PlayCookAnimation() override;
            virtual std::string GetCookbookAttachPointName() const override;
            virtual std::string GetCookPotAttachPointName() const override;

            virtual void PlayFoodThrowAnimation() override;
            virtual std::string GetFoodThrownItemAttachPointName() const override;
            virtual bool HasFoodThrowReleasePassed() const override;
            virtual bool HasFoodThrowAnimationFinished() const override;

            // Brewer interface
            virtual Brewer::Info PlayKegThrowdownAnimation() override;
            virtual std::string GetKegAttachPointName() const override;

            // LampHolder
            std::string GetLampAttachPoint() const override;

            // Reviveable
            bool IsDowned() const override;
            float Revive(Character* reviver) override;

        protected:
            static DwarfInfo CreateDefaultDwarfInfo(const CharacterParameters& parameters);

            virtual void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& inputDamage) override;
            virtual void OnPostInflictDamage(Character* target, const Vector2f& position, const Damage& damage) override;
            virtual void OnTargetKilled(Character* target) override;

            float OnPreInteractWithCharacter(Character* target) override;

            bool CanBeInteractedWith(const Character* interactor) const override;

            virtual Vector2f GetDeathImpulse(const Physics::Collision* collision) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            static const std::string NormalRunAnimation;
            static const std::string ForwardRunAnimation;
            static const std::string BackwardRunAnimation;

        private:
            float playEatAnimation();

            float playReiveAnimation();

            float playDrinkAnimation(DrinkableType type);
            void updateCupPositions();

            void down();
            float revive(Character* reviver);

            bool _downed;
            float _downedDeathTimer;
            Audio::SoundSet _downedSounds;

            bool _standUpQueued;

            bool _downedTransitioning;
            float _downedTransitionDuration;
            float _downedTransitionTimer;

            std::string _throwReleaseTag;
            Animation::AnimationSet _throwAnimations;
            Animation::AnimationSet _foodThrowAnimations;
            Animation::AnimationSet _incapacitatedEatAnimations;

            Animation::AnimationSet _leapAnimations;
            std::vector<Audio::SoundSet> _leapSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapTerrainSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _leapLandTerrainSounds;
            Audio::SoundSet _leapLandAlwaysSounds;

            Animation::AnimationSet _reviveAnimations;
            Animation::AnimationSet _standUpAnimations;
            Animation::AnimationSet _cookAnimations;
            Animation::AnimationSet _kegThrowndownAnimations;

            bool _eating;
            Animation::AnimationSet _eatAnimations;
            Animation::AnimationSet _eatIncapacitatedAnimations;
            Audio::SoundSet _eatingSounds;
            std::shared_ptr<Audio::ManagedSoundInstance> _curEatingSound;
            std::map<std::string, std::vector<std::string>> _eatAnimationTags;
            std::vector<Particles::ParticleSystemInstance*> _eatParticles;

            bool _drinking = false;
            DrinkableType _drinkingType;
            Animation::AnimationSet _drinkAnimations;
            Audio::SoundSet _drinkingSounds;
            std::map<DrinkableType, Animation::SkeletonInstance*> _drinkingCups;

            Particles::ParticleSystemInstance* _deathParticles = nullptr;

            const App::Cursor* _reviveCursor = nullptr;

            DwarfInfo _info;
        };
    }

    template <>
    void EnumeratePreloads<Character::Dwarf>(PreloadSet& preloads);
}

#include "Dwarf.inl"
