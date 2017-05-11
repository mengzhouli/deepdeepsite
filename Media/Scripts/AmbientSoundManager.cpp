#include "AmbientSoundManager.hpp"

namespace Dwarf
{
    namespace Audio
    {
        static const float AmbientAudioFadeTime = 5.0f;

        AmbientSoundManager::AmbientSoundManager(SoundManager* soundManager)
            : _soundManager(soundManager)
        {
        }

        void AmbientSoundManager::AddSounds(EnvironmentType environment, const SoundPathVector& sounds, float volume)
        {
            _environments[environment].sounds.AddSounds(sounds);
            _environments[environment].volume = volume;
        }

        void AmbientSoundManager::SetVolume(float volume)
        {
            _volume = volume;
        }

        float AmbientSoundManager::GetVolume() const
        {
            return _volume;
        }

        void AmbientSoundManager::LoadContent(Content::ContentManager* contentManager)
        {
            for (auto& env : _environments)
            {
                env.second.sounds.LoadContent(contentManager);
            }
        }

        void AmbientSoundManager::UnloadContent()
        {
            for (auto& env : _environments)
            {
                env.second.sounds.UnloadContent();
            }
        }

        void AmbientSoundManager::Update(double totalTime, float dt)
        {
            float deltaVolume = (dt / AmbientAudioFadeTime) * _volume;

            EnvironmentType curEnvironment = _soundManager->GetCurrentEnvironment();
            for (auto& env : _environments)
            {
                float envVolume = env.second.volume;
                if (env.first == curEnvironment)
                {
                    if (env.second.curSound == nullptr || env.second.curSound->GetStatus() == AudioStatus_Stopped)
                    {
                        env.second.curSound = _soundManager->PlayLoopingGlobalSound(env.second.sounds.GetNextSound(), SoundPriority::Low, 0.0f);
                    }

                    if (env.second.curSound != nullptr)
                    {
                        env.second.curSound->SetVolume(Min(env.second.curSound->GetVolume() + (deltaVolume * envVolume), _volume * envVolume));
                    }
                }
                else
                {
                    if (env.second.curSound == nullptr || env.second.curSound->GetStatus() == AudioStatus_Stopped)
                    {
                        env.second.curSound = _soundManager->PlayLoopingGlobalSound(env.second.sounds.GetNextSound(), SoundPriority::Low, 0.0f);
                    } 

                    if (env.second.curSound != nullptr)
                    {
                        env.second.curSound->SetVolume(Max(env.second.curSound->GetVolume() - (deltaVolume * envVolume), 0.0f));

                        if (env.second.curSound->GetVolume() <= 0.0f || env.second.curSound->GetStatus() == AudioStatus_Stopped)
                        {
                            env.second.curSound->Stop(0.0f);
                            env.second.curSound = nullptr;
                        }
                    }
                }
            }
        }

    }
}
