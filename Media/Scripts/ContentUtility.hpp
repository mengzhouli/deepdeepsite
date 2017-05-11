#pragma once

#include "Content/ContentManager.hpp"
#include "Application/Cursor.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "Audio/SoundInstance.hpp"

namespace Dwarf
{
    namespace Content
    {
        Animation::SkeletonInstance* CreateSkeletonInstance(ContentManager* contentManager, const std::string& skelPath);
        Animation::SkeletonInstance* CreateSkeletonInstance(ContentManager* contentManager, const std::string& skelPath, const std::string& matsetPath);

        Particles::ParticleSystemInstance* CreateParticleSystemInstance(ContentManager* contentManager, const std::string& path);

        Audio::SoundInstance* CreateSoundInstance(ContentManager* contentManager, const std::string& path);

        const App::Cursor* GetCursor(ContentManager* contentManager, const std::string& cursorSetPath, const std::string& cursorName);
    }
}
