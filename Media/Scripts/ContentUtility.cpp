#include "ContentUtility.hpp"
#include "Application/CursorSet.hpp"
#include "Audio/Sound.hpp"

namespace Dwarf
{
    namespace Content
    {
        Animation::SkeletonInstance* CreateSkeletonInstance(ContentManager* contentManager, const std::string& skelPath)
        {
            const Animation::Skeleton* skel = nullptr;
            try
            {
                skel = contentManager->Load<Animation::Skeleton>(skelPath);
                Animation::SkeletonInstance* instance = new Animation::SkeletonInstance(skel);
                SafeRelease(skel);

                return instance;
            }
            catch (...)
            {
                SafeRelease(skel);
                throw;
            }
        }

        Animation::SkeletonInstance* CreateSkeletonInstance(ContentManager* contentManager, const std::string& skelPath, const std::string& matsetPath)
        {
            const Animation::Skeleton* skel = nullptr;
            const Graphics::PolygonMaterialSet* matset = nullptr;
            try
            {
                skel = contentManager->Load<Animation::Skeleton>(skelPath);
                matset = contentManager->Load<Graphics::PolygonMaterialSet>(matsetPath);

                Animation::SkeletonInstance* instance = new Animation::SkeletonInstance(skel, matset);

                SafeRelease(skel);
                SafeRelease(matset);

                return instance;
            }
            catch (...)
            {
                SafeRelease(skel);
                SafeRelease(matset);

                throw;
            }
        }

        Particles::ParticleSystemInstance* CreateParticleSystemInstance(ContentManager* contentManager, const std::string& path)
        {
            const Particles::ParticleSystem* system = nullptr;
            try
            {
                system = contentManager->Load<Particles::ParticleSystem>(path);
                Particles::ParticleSystemInstance* instance = new Particles::ParticleSystemInstance(system);
                SafeRelease(system);
                return instance;
            }
            catch (...)
            {
                SafeRelease(system);

                throw;
            }
        }

        Audio::SoundInstance* CreateSoundInstance(ContentManager* contentManager, const std::string& path)
        {
            const Audio::Sound* sound = nullptr;
            try
            {
                sound = contentManager->Load<Audio::Sound>(path);
                Audio::SoundInstance* instance = sound->CreateInstance();
                SafeRelease(sound);
                return instance;
            }
            catch (...)
            {
                SafeRelease(sound);
                throw;
            }
        }

        const App::Cursor* GetCursor(ContentManager* contentManager, const std::string& cursorSetPath, const std::string& cursorName)
        {
            const App::CursorSet* cursors = contentManager->Load<App::CursorSet>(cursorSetPath);
            const App::Cursor* cursor = cursors->GetCursor(cursorName);
            SafeAddRef(cursor);
            SafeRelease(cursors);
            return cursor;
        }
    }
}
