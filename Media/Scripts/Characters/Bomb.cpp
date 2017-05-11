#include "Characters/Bomb.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector BombExplosionSoundPaths =
        {
            { "Audio/Characters/Bomb/Bomb1_MASTER.ogg", 1.0f },
            { "Audio/Characters/Bomb/Bomb1_wDebris_MASTER.ogg", 1.0f },
        };

        static const Audio::SoundPathVector BombBounceSoundPaths = 
        {
            { "Audio/Characters/Bomb/Bomb_bouncing.ogg", 1.0f },
        };
        static const std::pair<float, float> BounceSoundRadius = { 500, 3500 };
        static const float BounceSoundVolume = 1.0f;

        Bomb::Bomb(const CharacterParameters& parameters, const std::string& skel, const std::string& matset, const std::string& fuseName,
                   const std::string& exposionOrigin, CharacterID owner, float duration, float radius, float dmg)
            : Explosive(parameters, skel, matset, fuseName, exposionOrigin, owner, duration, radius, dmg)
        {
            SetDrawSkeletonAfterExplosion(false);
            SetAttachToGroundOnSpawn(false);

            AddExplosionSounds(BombExplosionSoundPaths);

            _bounceSounds.AddSounds(BombBounceSoundPaths);
        }

        void Bomb::OnUpdate(double totalTime, float dt)
        {
            Explosive::OnUpdate(totalTime, dt);

            CharacterState curState = GetCurrentState();
            if (curState == CharacterState_Physics && !HasExploded())
            {
                const Physics::Collision* collision = GetCollision();
                assert(collision);

                if (!collision->GetCurrentContacts().empty())
                {
                    auto soundManager = GetLevel()->GetSoundManager();
                    soundManager->PlaySinglePositionalSound(_bounceSounds.GetNextSound(), Audio::SoundPriority::Low, GetPosition(), BounceSoundRadius.first, BounceSoundRadius.second, BounceSoundVolume);
                }
            }
        }

        void Bomb::OnLoadContent(Content::ContentManager* contentManager)
        {
            Explosive::OnLoadContent(contentManager);

            _bounceSounds.LoadContent(contentManager);
        }

        void Bomb::OnUnloadContent()
        {
            Explosive::OnUnloadContent();

            _bounceSounds.UnloadContent();
        }

        static const std::string IronBombSkeletonPath = "Skeletons/Characters/Bomb/ironbomb.skel";
        static const std::string IronBombMatsetPath = "Skeletons/Characters/Bomb/ironbomb.polymatset";

        IronBomb::IronBomb(const CharacterParameters& parameters, CharacterID owner, float duration, float radius, float dmg)
            : Bomb(parameters, IronBombSkeletonPath, IronBombMatsetPath, "fuse", "origin", owner, duration, radius, dmg)
        {
        }
    }

    template <>
    void EnumeratePreloads<Character::Bomb>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Explosive>(preloads);
        EnumerateSoundVectorPreloads(preloads, Character::BombExplosionSoundPaths);
        EnumerateSoundVectorPreloads(preloads, Character::BombBounceSoundPaths);
    }

    template <>
    void EnumeratePreloads<Character::IronBomb>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Bomb>(preloads);

        preloads.insert(Character::IronBombSkeletonPath);
        preloads.insert(Character::IronBombMatsetPath);
    }
}
