#pragma once

#include "Characters/SkeletonCharacter.hpp"

#include "SoundSet.hpp"
#include "Characters/DamageSounds.hpp"
#include "EmoteTypes.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class OverheadTextDisplay;
        class EmoteDisplay;
    }

    namespace Particles
    {
        class ParticleSystem;
        class ParticleSystemInstance;
    }
    namespace Character
    {
        enum AnimationType
        {
            AnimationType_Idle = 0,
            AnimationType_Move = 1,
        };

        class BasicCharacter : public SkeletonCharacter
        {
        public:
            BasicCharacter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath);

            void LookAt(const Vector2f& position);
            void LookTo(const Vector2f& direction);
            void StopLooking();

            void PointMainArmAt(const Vector2f& position);
            void PointMainArmTo(const Vector2f& direction);
            void StopPointingMainArm();

            void PointOffArmAt(const Vector2f& position);
            void PointOffArmTo(const Vector2f& direction);
            void StopPointingOffArm();

            bool HasRangedAttackFireAnimationPassed(const Item::Weapon* weap) const;

            void AddOverheadText(const Graphics::PreparedText& text);

            void Emote(EmoteType type, float duration);

            std::shared_ptr<Audio::ManagedSoundInstance> Speak(const Audio::SoundSet& sounds, bool loop, bool positional);
            std::shared_ptr<Audio::ManagedSoundInstance> Speak(SpeechType type, bool positional);

            void SetAggroRange(float range);
            virtual float GetAggroRange() const override;

            virtual MaterialType GetBodyMaterial() const override;

            Vector2f GetMouthPosition() const;
            Vector2f GetHeadPosition() const;
            Vector2f GetFeetPosition() const;

            bool WantsToLookAtMouse() const;

            void SetWeaponAlpha(float alpha, float time);

        protected:
            void AddWeaponAttachment(const std::string& name, const std::string attachA, const std::string attachB,
                                     const std::map<Item::WeaponType, std::string>& weapTypesAttachJoints);
            void ClearWeaponAttachments();

            struct AttackAnimation
            {
                Item::WeaponType Type;
                std::vector<std::pair<std::string, std::string>> DamageRangeTags;
                std::vector<std::string> DamageResetTags;
                std::vector<std::string> AttackSoundTags;
                std::vector<std::string> WooshSoundTags;
            };

            using AttackRange = std::pair<float, float>;

            void AddAttackAnimation(const std::string& animName, float weight,
                                    const std::map<std::string, AttackAnimation>& attachments, 
                                    const AttackRange& attackRange = AttackRange(0.0f, std::numeric_limits<float>::max()));
            void ClearAttackAnimations();

            void AddInteractAnimation(const std::string& animName, float weight);

            void SetMouthName(const std::string& jointName);
            void SetHeadName(const std::string& jointName);
            void AddMainArmJoint(const std::string& jointName);
            void AddOffArmJoint(const std::string& jointName);
            void AddFootJoint(const std::string& jointAName, const std::string& jointBName);
            void SetJawJoint(const std::string& jointName);

            void AddTerrainAnimation(AnimationType type, Pathfinding::EdgeType edgeType, Pathfinding::TerrainType terrainType,
                                     float minAngle, AnimationVariant variant, const std::string& animationName, float weight);
            void ClearTerrainAnimations(AnimationType type, Pathfinding::EdgeType edgeType);

            void AddDamagedParticleSystem(const DamageType& damageType, const std::string& particleSystemPath, const std::string& spawnJoint = "");
            void AddHealingParticleSystem(const std::string& particleSystemPath);

            void SetRunParticleSystem(const std::string& particleSystemPath);
            void AddRunParticleTag(const std::string& animationTagName);

            void AddAggroSounds(const Audio::SoundPathVector& sounds);
            void AddAttackSounds(const Audio::SoundPathVector& sounds);
            void AddAttackWooshSounds(Item::WeaponType weaponType, const Audio::SoundPathVector& sounds);
            void AddAttackWooshSounds(const std::map<Item::WeaponType, Audio::SoundPathVector>& sounds);
            void SetAttackSoundPlayChance(float chance);
            void AddIdleSounds(const Audio::SoundPathVector& sounds);
            void SetIdleSoundInterval(float minTimeout, float maxTimeout);
            void AddDeathSounds(const Audio::SoundPathVector& sounds);
            void AddAffirmativeSounds(const Audio::SoundPathVector& sounds);
            void AddNegatorySounds(const Audio::SoundPathVector& sounds);
            void AddTauntSounds(const Audio::SoundPathVector& sounds);

            void SetGlobalSpeechParameters(float volume);
            void SetPositionalSpeechParameters(float volume, float minDist, float maxDist);

            void AddDamageSounds(const Audio::DamageSoundPaths& sounds);
            void AddDamageSounds(const std::vector<Audio::DamageSoundPaths>& sounds);

            void AddHealingSounds(const Audio::SoundPathVector& sounds);

            void AddBaseFootstepSounds(const Audio::SoundPathVector& sounds);
            void AddTerrainFootstepSounds(Pathfinding::TerrainType terrainType, const Audio::SoundPathVector& sounds);
            void SetFootstepVolumeRange(float minVolume, float maxVolume);
            void EnableFootstepCameraShake(float magnitude, float frequency);
            void DisableFootstepCameraShake();

            void SetDropWeaponsOnDeath(bool drop);
            void SetDropArmorsOnDeath(bool drop);
            void SetDropTrinketsOnDeath(bool drop);
            void SetDestroyItemsOnDeath(bool destroy);

            void SetFallDamage(float startVelocity, float velocityMult);

            void SetBodyMaterial(MaterialType material);

            virtual void OnScaleChange(float oldScale, float newScale) override;

            virtual Damage OnPreRecieveDamage(Character* source, const Vector2f& pos, const Damage& dmg) override;
            virtual void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

            virtual float OnPreInteractWithCharacter(Character* target) override;
            virtual float OnPreInteractWithItem(Item::Item* target) override;

            virtual void OnPostHealed(Character* source, float ammount) override;

            virtual void OnStateChange(CharacterState newState) override;

            virtual void OnEquipItem(Item::Item* item) override;
            virtual void OnUnequipItem(Item::Item* item) override;

            virtual Vector2f GetLineOfSightPoint() const override;

            virtual void OnSpawn() override;
            virtual void OnDeath() override;

            virtual AttackInfo AttackTarget(Character* target) override;
            virtual bool IsInAttackDmgRange(const Item::Weapon* weap) const override;

            virtual Rayf GetWeaponAttachPoint(const std::string& name, Item::WeaponType type) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            void DoDeathRagdoll(bool forcedPhysics);
            void DoDeathDropItems();

            virtual Vector2f GetDeathImpulse(const Physics::Collision* collision) const;

        private:
            void aggroNearbyFriendlies(const Character* target) const;

            Vector2f getDeathPosition() const;

            template <typename T>
            void doDeathDropItemsImpl(bool drop, bool destroy);

            void lookAt(const Vector2f& position, bool forced);
            void lookTo(const Vector2f& direction, bool forced);
            void stopLooking(bool forced);

            const Animation::AnimationSet& getTerrainAnimation(AnimationType type, Pathfinding::EdgeType edgeType,
                                                               Pathfinding::TerrainType terrainType, float angle, AnimationVariant variant) const;
            const Animation::AnimationSet& getCurrentTerrainAnimation(AnimationType type) const;

            void updateMoveAnimation();
            void updateIdleAnimation();

            void assignWeapons();

            const Audio::SoundSet& getSoundsForSpeech(SpeechType type) const;

            struct weaponAttachment
            {
                std::string attachA;
                std::string attachB;
                std::map<Item::WeaponType, std::string> jointNames;
                Item::WeaponType equippedWeaponType;
                Item::ItemID equippedWeapon;
            };
            std::map<std::string, weaponAttachment> _weaponAttachmentLocations;
            std::map<Item::WeaponType, std::vector<std::string>> _weaponAttachmentTypes;

            struct attackAnimationRequirements
            {
                std::map<std::string, Item::WeaponType> requiredAttachments;
                std::map<std::string, float> animationWeights;
                std::map<std::string, std::map<std::string, std::vector<std::pair<std::string, std::string>>>> animationDmgRanges;
                std::map<std::string, std::map<std::string, std::vector<std::string>>> animationDmgResetTags;
                std::map<std::string, std::map<std::string, std::vector<std::string>>> animationAttackSoundTags;
                std::map<std::string, std::map<std::string, std::vector<std::string>>> animationWooshSoundTags;
                std::map<std::string, AttackRange> animationRanges;
            };
            std::vector<attackAnimationRequirements> _attackAnimations;

            std::unordered_map<Item::ItemID, std::vector<std::pair<std::string, std::string>>> _curDmgRangeTags;
            std::unordered_map<Item::ItemID, std::vector<std::string>> _curDmgResetTags;
            std::set<std::string> _curAttackSoundTags;
            std::map<std::string, Item::WeaponType> _curWooshSoundTags;

            std::string _mouthName;
            std::string _headName;
            float _headRotationSpeed;
            bool _hasForcedLookPos;

            std::vector<std::string> _mainArmNames;
            std::vector<std::string> _offArmNames;
            float _armRotationSpeed;

            struct damageParticles
            {
                DamageType damageType = 0;
                std::string path = "";
                Particles::ParticleSystemInstance* particles = nullptr;
                std::string spawnJoint = "";
            };
            std::vector<damageParticles> _damageParticles;
            
            std::string _healParticlesPath = "";
            Particles::ParticleSystemInstance* _healParticles = nullptr;

            std::string _runParticleSystemPath;
            const Particles::ParticleSystem* _runParticles;
            std::vector<std::string> _runParticleSystemTags;

            struct foot
            {
                std::string front;
                std::string back;
                Particles::ParticleSystemInstance* particleSystem;
            };
            std::vector<foot> _feet;

            Audio::SoundSet _baseFootstepSounds;
            std::map<Pathfinding::TerrainType, Audio::SoundSet> _terrainFootstepSounds;
            std::pair<float, float> _footstepVolumeRange = std::make_pair(1.0f, 1.0f);
            bool _footstepCameraShakeEnabled = false;
            float _footstepCameraShakeMagnitude = 0.0f;
            float _footstepCameraShakeFrequency = 0.0f;

            std::string _jawName;
            float _remainingJawOpenDuration;

            using animationVariantMap = std::map<AnimationVariant, Animation::AnimationSet>;
            using animationAngleMap = std::map<float, animationVariantMap>;
            using terrainTypeAnimations = std::map<Pathfinding::TerrainType, animationAngleMap>;
            using edgeTypeAnimations = std::map<Pathfinding::EdgeType, terrainTypeAnimations>;
            using terrainAnimations = std::map<AnimationType, edgeTypeAnimations>;
            terrainAnimations _terrainAnimations;

            Animation::AnimationSet _interactAnimations;
            Animation::AnimationSet _deathAnimations;

            Audio::SoundSet _aggroSounds;
            Audio::SoundSet _attackSounds;
            std::map<Item::WeaponType, Audio::SoundSet> _attackWooshSounds;
            float _attackSoundPlayChance; 
            Audio::SoundSet _idleSounds;
            std::pair<float, float> _idleSoundTimeout = std::make_pair(10, 30);
            float _idleSoundTimer = 0.0f;
            Audio::SoundSet _deathSounds;
            Audio::SoundSet _affirmativeSounds;
            Audio::SoundSet _negatorySounds;
            Audio::SoundSet _tauntSounds;
            float _speechMinDist;
            float _speechMaxDist;
            float _speechPositionalVolume = 1.0f;
            float _speechGlobalVolume = 1.0f;

            struct damageSoundSet
            {
                MaterialType materialType;
                DamageType damageType;
                Audio::SoundSet sounds;
            };
            std::vector<damageSoundSet> _damageSounds;
            Audio::SoundSet _healingSounds;

            std::vector<std::weak_ptr<Audio::ManagedSoundInstance>> _vocalizations;

            MaterialType _bodyMaterial;

            float _aggroRange;

            float _injuredThreshold;

            float _physicsTime;

            bool _dieing;
            bool _ragDollOnDeath;

            float _deathFadeBegin;
            float _deathFadeDuration;
            float _timeDead;

            bool _dropWeaponsOnDeath;
            bool _dropArmorOnDeath;
            bool _dropTrinketsOnDeath;
            bool _destroyItemsOnDeath;

            Pathfinding::EdgeType _curPathEdgeType;

            Pathfinding::EdgeType _curEdgeType;
            bool _startingClimb;
            bool _endingSlide;

            float _fallDmgStartVelocity;
            float _fallDmgMult;

            CharacterID _lastAttacker;
            Vector2f _lastDmgPos;

            const Graphics::Font* _damageFont;
            const Graphics::Font* _healingFont;

            Graphics::OverheadTextDisplay* _textDisplay;
            Graphics::EmoteDisplay* _emoteDisplay;

            float _targetWeaponAlpha = 1.0f;
            float _weaponAlphaTimer = 0.0f;
            float _weaponAlphaTotalTime = 0.0f;
        };
    }

    template <>
    void EnumeratePreloads<Character::BasicCharacter>(PreloadSet& preloads);
}
