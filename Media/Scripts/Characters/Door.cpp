#include "Characters/Door.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string DoorSkeletonPath = "Skeletons/Items/Doodads/door.skel";
        static const std::string DoorMaterialPath = "Skeletons/Items/Doodads/door.polymatset";
        static const std::string DoorHitParticlesPath = "Particles/door_dust.partsys";

        static const std::string DoorOpenAnimation = "open";
        static const std::string DoorCloseAnimation = "close";
        static const std::pair<std::string, std::string> DoorParticleJoints = std::make_pair("particles_base", "particles_end");
        static const std::string DoorHitTag = "hit";
        static const std::string DoorChainTag = "chain";

        static const std::string DoorChainSoundOriginJoint = "chain_origin";
        static const std::string DoorHitSoundOriginJoint = "sound_origin";
        static const std::pair<float, float> DoorSoundRadius = std::make_pair(1000.0f, 3000.0f);
        static const float DoorSoundVolume = 1.0f;

        static const float DoorShakeMagnitude = 5.0f;
        static const float DoorShakeFrequency = 10.0f;
        static const float DoorShakeDuration = 0.75f;

        static const std::vector<Audio::SoundPathVector> DoorHitSoundPaths =
        {
            {
                { "Audio/Footsteps/step_Land_OrcAlways_1.wav", 1.0f },
                { "Audio/Footsteps/step_Land_OrcAlways_2.wav", 1.0f },
            },
            {
                { "Audio/Items/Weapons/Ork_Sword_Swipe.wav", 1.0f },
            },
        };

        static const Audio::SoundPathVector DoorChainSoundPaths =
        {
            { "Audio/Characters/Chain/Chain_drop_1.wav", 1.0f },
            { "Audio/Characters/Chain/Chain_drop_2.wav", 1.0f },
        };

        Door::Door(const CharacterParameters& parameters, DoorSize size, const std::string& collisionClosedName, const std::string& collisionOpenName, bool startOpen, bool inverted, OpenStateChangeCallback callback) : SkeletonCharacter(parameters, DoorSkeletonPath, DoorMaterialPath)
            , _size(size)
            , _inverted(inverted)
            , _startOpen(startOpen)
            , _state(!startOpen)
            , _collisionClosedName(collisionClosedName)
            , _collisionOpenName(collisionOpenName)
            , _callback(callback)
        {
            SetEntityMask(CharacterMask_None);
            SetMoveType(MoveType_None);
            SetAttachToGroundOnSpawn(false);
            SetSkeletonCastsShadows(true);
            SetSkeletonInvertsWithDirection(false);

            SetTooltip("character_door_name");

            DisablePhysics();
            SetInvulnerable(true);

            for (const auto& doorHitSounds : DoorHitSoundPaths)
            {
                _doorHitSounds.push_back(Audio::SoundSet(doorHitSounds));
            }
            _doorChainSounds.AddSounds(DoorChainSoundPaths);
        }

        float Door::SetOpen(bool open)
        {
            if (_state != open)
            {
                if (_callback != nullptr && !_callback(open))
                {
                    return 0.0f;
                }

                _state = open;
                const std::string& anim = _state ? DoorOpenAnimation : DoorCloseAnimation;
                PlayAnimation(anim, false, 0.0f, 0.0f);
                GetLevelLayer()->SetTerrainCollisionEnabled(_collisionClosedName, !_state);
                GetLevelLayer()->SetTerrainCollisionEnabled(_collisionOpenName, _state);

                SetSkeletonCastsShadows(!open);

                return GetAnimationLength(anim);
            }
            else
            {
                return 0.0f;
            }
        }

        bool Door::IsOpen() const
        {
            return _state;
        }

        void Door::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();

            float scale = 1.0f;
            switch (_size)
            {
            case DoorSize_Small: scale = 0.5f; break;
            case DoorSize_Medium: scale = 1.0f; break;
            case DoorSize_Large: scale = 1.5f; break;
            case DoorSize_Huge: scale = 2.5f; break;
            default: assert(0);
            }

            SetScale(scale);

            GetSkeleton()->SetInvertedX(_inverted);

            PlayAnimation(DoorCloseAnimation, false, 0.0f, 0.0f);

            SetOpen(_startOpen);
            Update(0.0, 1000.0f);
        }

        void Door::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            _hitParticles = Content::CreateParticleSystemInstance(contentManager, DoorHitParticlesPath);
            AttachDrawable(DoorParticleJoints.first, _hitParticles);

            for (auto& doorHitSounds : _doorHitSounds)
            {
                doorHitSounds.LoadContent(contentManager);
            }
            _doorChainSounds.LoadContent(contentManager);
        }

        void Door::OnUnloadContent()
        {
            DetachDrawable(DoorParticleJoints.first, _hitParticles);

            SkeletonCharacter::OnUnloadContent();

            SafeRelease(_hitParticles);

            for (auto& doorHitSounds : _doorHitSounds)
            {
                doorHitSounds.UnloadContent();
            }
            _doorChainSounds.UnloadContent();
        }

        void Door::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            if (IsPlayingAnimation() && HasAnimationTagJustPassed(DoorHitTag))
            {
                const auto& skeleton = GetSkeleton();

                const Vector2f& particleBase = skeleton->GetJointPosition(DoorParticleJoints.first);
                const Vector2f& particleEnd = skeleton->GetJointPosition(DoorParticleJoints.second);
                Rotatorf rotation(particleBase - particleEnd);
                _hitParticles->SetLineSpawner(particleBase, particleEnd);
                _hitParticles->SetRotation(rotation);
                _hitParticles->SetInvertedX(skeleton->IsInvertedX());
                _hitParticles->SetInvertedY(skeleton->IsInvertedY());
                _hitParticles->Burst();

                const Vector2f& soundOrigin = skeleton->GetJointPosition(DoorHitSoundOriginJoint);

                auto soundManager = GetLevel()->GetSoundManager();

                for (const auto& doorHitSounds : _doorHitSounds)
                {
                    soundManager->PlaySinglePositionalSound(doorHitSounds.GetNextSound(), Audio::SoundPriority::High, soundOrigin, DoorSoundRadius.first, DoorSoundRadius.second, DoorSoundVolume);
                }

                auto& cameraController = GetLevel()->GetCameraController();
                cameraController.Shake(soundOrigin, DoorSoundRadius.first, DoorSoundRadius.second, DoorShakeMagnitude, DoorShakeFrequency, DoorShakeDuration);
            }

            if (IsPlayingAnimation() && HasAnimationTagJustPassed(DoorChainTag))
            {
                const auto& skeleton = GetSkeleton();
                const Vector2f& soundOrigin = skeleton->GetJointPosition(DoorChainSoundOriginJoint);

                auto soundManager = GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(_doorChainSounds.GetNextSound(), Audio::SoundPriority::High, soundOrigin, DoorSoundRadius.first, DoorSoundRadius.second, DoorSoundVolume);
            }

            _hitParticles->Update(totalTime, dt);
        }

        void Door::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);
        }
    }

    template <>
    void EnumeratePreloads<Character::Door>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert(Character::DoorSkeletonPath);
        preloads.insert(Character::DoorMaterialPath);

        preloads.insert(Character::DoorHitParticlesPath);

        for (const auto& doorHitSounds : Character::DoorHitSoundPaths)
        {
            EnumerateSoundVectorPreloads(preloads, doorHitSounds);
        }
        EnumerateSoundVectorPreloads(preloads, Character::DoorChainSoundPaths);
    }
}
