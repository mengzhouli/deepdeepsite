#include "Characters/Checkpoint.hpp"

#include "Drawables/Flame.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string CheckpointSkeletonPath = "Skeletons/Characters/Checkpoint/checkpoint.skel";
        static const std::string CheckpointMaterialPath = "Skeletons/Characters/Checkpoint/checkpoint.polymatset";

        static const std::string CheckpointIdleAnimation = "unconstructed_idle";
        static const std::string CheckpointOpenAnimation = "construct";

        static const std::string CheckpointStartTorchTag = "particle";

        static const std::vector<std::string> FlameAttachPoints
        {
            "particle0",
            "particle1",
        };

        static const std::string CheckpointAttachedSkeletonsSizeJoint = "background";
        static const std::map<std::string, std::pair<std::pair<std::string, std::string>, std::vector<std::pair<std::string, float>>>> CheckpointSkeletonAttachments
        {
            { "bartender", { { "Skeletons/Characters/Dwarf/dwarf.skel", "Skeletons/Characters/Dwarf/bartender.polymatset" }, { { "idle", 0.97f }, { "dance", 0.03f } } } },
            { "barfly_0", { { "Skeletons/Characters/Pdwarfs/bardwarf1.skel", "Skeletons/Characters/Pdwarfs/bardwarf1_1.polymatset" }, { { "idle1", 0.7f }, { "idle2", 0.1f }, { "idle3", 0.1f }, { "idle4", 0.1f }, } } },
            { "barfly_1", { { "Skeletons/Characters/Pdwarfs/bardwarf1.skel", "Skeletons/Characters/Pdwarfs/bardwarf1_2.polymatset" }, { { "idle1", 0.7f }, { "idle2", 0.1f }, { "idle3", 0.1f }, { "idle4", 0.1f }, } } },
            { "dwarf_table", { { "Skeletons/Characters/Pdwarfs/baldwarfs.skel", "Skeletons/Characters/Pdwarfs/baldwarfs.polymatset" }, { { "idle1", 0.6 }, { "idle2", 0.1f }, { "idle3", 0.1f }, { "idle4", 0.1f }, { "idle5", 0.1f }, } } },
        };

        static const std::map<std::string, float> CheckpointAttachedSkeletonScales
        {
            { "bartender", 0.6f },
            { "barfly_0", 1.0f },
            { "barfly_1", 1.0f },
            { "dwarf_table", 1.0f },
        };

        static const float CheckpointOpenSoundVolume = 0.6f;
        static const Audio::SoundPathVector CheckpointOpeningSoundPaths =
        {
            { "Audio/Characters/Checkpoint/Checkpoint_RockBuild.ogg", 1.0f },
        };

        static const float CheckpointIdleSoundVolume = 0.4f;
        static const Audio::SoundPathVector CheckpointIdleSoundPaths =
        {
        };

        Checkpoint::Checkpoint(const CharacterParameters& parameters, const Color& flameColor, OnCheckpointUseCallback callback, bool initialState, bool captureOnFirstUpdate)
            : SkeletonCharacter(parameters, CheckpointSkeletonPath, CheckpointMaterialPath)
            , _initialState(initialState)
            , _state(initialState)
            , _captureOnFirstUpdate(captureOnFirstUpdate)
            , _callback(callback)
            , _openSounds(CheckpointOpeningSoundPaths)
            , _idleSounds(CheckpointIdleSoundPaths)
            , _curIdleSound()
            , _flames()
            , _attachedSkeletons()
        {
            SetEntityMask(CharacterMask_Usable);
            SetMoveType(MoveType_None);

            SetTooltip("character_checkpoint_name", GetLevel()->GetStringTable()->GetString(parameters.Name).c_str());

            DisablePhysics();
            SetInvulnerable(true);

            for (const auto& flameAttachPt : FlameAttachPoints)
            {
                _flames.push_back(std::make_pair(flameAttachPt, std::make_shared<Graphics::Flame>(flameColor, false, false, true, true, GetLevel()->GetSoundManager())));
            }
        }

        float Checkpoint::OnPreInteractedWith(Character* interactor)
        {
            if (_state)
            {
                return -1.0f;
            }

            return 0.0f;
        }

        void Checkpoint::OnPostInteractedWith(Character* interactor)
        {
            if (_state)
            {
                return;
            }

            if (_callback)
            {
                _callback(interactor);
            }

            _state = true;
            if (!_captureOnFirstUpdate)
            {
                PlayAnimation(CheckpointOpenAnimation, false, 0.0f, 0.0f);
            }

            auto soundPos = getAudioPosition();
            auto soundSize = getAudioRadius();

            auto soundManager = GetLevel()->GetSoundManager();
            if (!_initialState && !_captureOnFirstUpdate)
            {
                soundManager->PlaySinglePositionalSound(_openSounds.GetNextSound(), Audio::SoundPriority::High, soundPos, soundSize.first, soundSize.second, CheckpointOpenSoundVolume);
            }
            _curIdleSound = soundManager->PlayLoopingPositionalSound(_idleSounds.GetNextSound(), Audio::SoundPriority::High, soundPos, soundSize.first, soundSize.second, CheckpointIdleSoundVolume);
        }

        void Checkpoint::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            for (auto& flame : _flames)
            {
                flame.second->LoadContent(contentManager);
                AttachDrawable(flame.first, flame.second.get());
            }

            for (const auto& skeleton : CheckpointSkeletonAttachments)
            {
                Animation::SkeletonInstance* instance = Content::CreateSkeletonInstance(contentManager, skeleton.second.first.first, skeleton.second.first.second);
                AttachDrawable(skeleton.first, instance);

                Animation::AnimationSet animSet;
                for (const auto& anim : skeleton.second.second)
                {
                    animSet.AddAnimation(anim.first, anim.second);
                }
                instance->PlayAnimationSet(animSet, true);

                _attachedSkeletons.push_back(std::make_pair(skeleton.first, instance));
            }

            _openSounds.LoadContent(contentManager);
            _idleSounds.LoadContent(contentManager);
        }

        void Checkpoint::OnUnloadContent()
        {
            for (auto& flame : _flames)
            {
                flame.second->UnloadContent();
                DetachDrawable(flame.first, flame.second.get());
            }

            for (auto& skeleton : _attachedSkeletons)
            {
                SafeRelease(skeleton.second);
            }
            _attachedSkeletons.clear();

            _openSounds.UnloadContent();
            _idleSounds.UnloadContent();
            _curIdleSound = nullptr;

            SkeletonCharacter::OnUnloadContent();
        }

        void Checkpoint::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();

            if (_initialState || _captureOnFirstUpdate)
            {
                PlayAnimation(CheckpointOpenAnimation, false, 0.0f, GetAnimationLength(CheckpointOpenAnimation));
            }
            else
            {
                PlayAnimation(CheckpointIdleAnimation, true, 0.0f, 0.0f);
            }
        }

        void Checkpoint::OnDeath()
        {
            if (_curIdleSound)
            {
                _curIdleSound->Stop(0.25f);
                _curIdleSound = nullptr;
            }

            SkeletonCharacter::OnDeath();
        }

        void Checkpoint::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            if (!_state && _captureOnFirstUpdate)
            {
                OnPostInteractedWith(this);
            }

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            for (auto& flame : _flames)
            {
                flame.second->SetPosition(skeleton->GetJointPosition(flame.first));
                flame.second->SetScale(GetScale() * 0.75f);
                flame.second->Update(totalTime, dt);
            }

            if (skeleton->GetCurrentAnimation() == CheckpointOpenAnimation &&
                HasAnimationTagPassed(CheckpointStartTorchTag))
            {
                for (auto& flame : _flames)
                {
                    flame.second->SetState(true, !_initialState && !_captureOnFirstUpdate);
                }
            }

            const auto& scaleTransform = skeleton->GetJointTransformation(CheckpointAttachedSkeletonsSizeJoint);
            float attachedSkeletonScale = scaleTransform.Scale.X;

            for (auto& attachedSkeleton : _attachedSkeletons)
            {
                attachedSkeleton.second->SetScale(attachedSkeletonScale * CheckpointAttachedSkeletonScales.at(attachedSkeleton.first));
                attachedSkeleton.second->SetPosition(skeleton->GetJointPosition(attachedSkeleton.first));
                attachedSkeleton.second->Update(totalTime, dt);
            }

            if (_curIdleSound)
            {
                auto soundPos = getAudioPosition();
                auto soundSize = getAudioRadius();

                _curIdleSound->SetPosition(soundPos);
                _curIdleSound->SetMinMaxDistance(soundSize.first, soundSize.second);
            }
        }

        void Checkpoint::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            for (const auto& flame : _flames)
            {
                if (flame.second->ShouldDrawLights())
                {
                    levelRenderer->AddLight(flame.second->GetCenterLight());
                    levelRenderer->AddLight(flame.second->GetMainLight());
                }
            }
        }

        const Vector2f& Checkpoint::getAudioPosition() const
        {
            auto skeleton = GetSkeleton();
            const Vector2f& soundOrigin = skeleton->GetJointPosition("sound_origin");
            return soundOrigin;
        }

        std::pair<float, float> Checkpoint::getAudioRadius() const
        {
            auto skeleton = GetSkeleton();
            float soundSize = Vector2f::Distance(getAudioPosition(), skeleton->GetJointPosition("sound_size"));
            return std::make_pair(soundSize * 0.5f, soundSize * 3.0f);
        }
    }

    template <>
    void EnumeratePreloads<Character::Checkpoint>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert(Character::CheckpointSkeletonPath);
        preloads.insert(Character::CheckpointMaterialPath);

        for (const auto& skeleton : Character::CheckpointSkeletonAttachments)
        {
            preloads.insert(skeleton.second.first.first);
            preloads.insert(skeleton.second.first.second);
        }

        EnumerateSoundVectorPreloads(preloads, Character::CheckpointOpeningSoundPaths);
        EnumerateSoundVectorPreloads(preloads, Character::CheckpointIdleSoundPaths);
    }
}
