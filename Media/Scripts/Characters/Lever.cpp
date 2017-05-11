#include "Characters/Lever.hpp"

namespace Dwarf
{
    namespace Character
    {
        const std::string LeverSkeletonPath = "Skeletons/Items/Doodads/lever.skel";
        const std::string LeverMaterialPath = "Skeletons/Items/Doodads/lever.polymatset";

        static const Audio::SoundPathVector LeverMoveSounds = 
        {
            { "Audio/Characters/Lever/Lever1000.ogg", 1.0f },
        };
        static const float LeverMoveSoundVolume = 1.0f;
        static const std::pair<float, float> LeverMoveSoundRadius = std::make_pair(1000.0f, 3000.0f);

        const std::string LeverOpenAnimation = "open";
        const std::string LeverCloseAnimation = "close";

        Lever::Lever(const CharacterParameters& parameters, OnLeverUseCallback callback, bool initialState)
            : SkeletonCharacter(parameters, LeverSkeletonPath, LeverMaterialPath)
            , _state(initialState)
            , _callback(callback)
        {
            SetEntityMask(CharacterMask_Usable);
            SetMoveType(MoveType_None);

            SetTooltip("character_lever_name");

            DisablePhysics();
            SetInvulnerable(true);

            _moveSounds.AddSounds(LeverMoveSounds);
        }

        float Lever::OnPreInteractedWith(Character* interactor)
        {
            auto soundManager = GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_moveSounds.GetNextSound(), Audio::SoundPriority::Medium, GetPosition(),
                LeverMoveSoundRadius.first, LeverMoveSoundRadius.second, LeverMoveSoundVolume);

            const std::string anim = _state ? LeverCloseAnimation : LeverOpenAnimation;

            _interactor = interactor->GetID();
            _interactionTimer = PlayAnimation(anim, false, 0.0f, 0.0f);

            return _interactionTimer;
        }

        void Lever::OnPostInteractedWith(Character* interactor)
        {
        }

        void Lever::OnLoadContent(Content::ContentManager* contentManager)
        {
            _moveSounds.LoadContent(contentManager);

            SkeletonCharacter::OnLoadContent(contentManager);
        }

        void Lever::OnUnloadContent()
        {
            _moveSounds.UnloadContent();

            SkeletonCharacter::OnUnloadContent();
        }

        void Lever::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();

            const std::string anim = _state ? LeverOpenAnimation : LeverCloseAnimation;
            PlayAnimation(anim, false, 0.0f, GetAnimationLength(anim));
        }

        void Lever::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            if (_interactionTimer >= 0.0f)
            {
                _interactionTimer -= dt;
                if (_interactionTimer < 0.0f)
                {
                    if (_callback)
                    {
                        _callback(GetLevelLayer()->GetCharacter(_interactor), _state);
                    }

                    _state = !_state;
                }
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::Lever>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert(Character::LeverSkeletonPath);
        preloads.insert(Character::LeverMaterialPath);

        EnumerateSoundVectorPreloads(preloads, Character::LeverMoveSounds);
    }
}
