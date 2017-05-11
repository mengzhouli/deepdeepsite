#include "Audio/LavaSound.hpp"
#include "ContentUtility.hpp"
#include "MathUtility.hpp"
#include "Audio/Sound.hpp"

namespace Dwarf
{
    namespace Audio
    {
        static const std::string LavaSoundPath = "Audio/Ambiance/Lava_loop.ogg";

        LavaSound::LavaSound(Content::ContentManager* cm, SoundManager* manager, const Chainf& path, float minDistance, float maxDistance)
            : _playing(false)
            , _timer(0.0f)
            , _totalTime(0.0f)
        
            , _minDist(minDistance)
            , _maxDist(maxDistance)

            , _soundManager(manager)
            , _sound(nullptr)
            , _soundInstance(nullptr)
            , _path(path)
        {
            _sound = cm->Load<Sound>(LavaSoundPath);
            assert(_path.Size() >= 2);
        }

        LavaSound::~LavaSound()
        {
            SafeRelease(_sound);
        }

        void LavaSound::Play(float rampUpTime)
        {
            _timer = 0.0f;
            _totalTime = rampUpTime;
            _playing = true;
        }

        void LavaSound::Update(double totalTime, float dt, const Camera& camera)
        {
            if (_playing)
            {
                _timer += dt;
                float volume = _totalTime > 0.0f ? Clamp(_timer / _totalTime, 0.0f, 1.0f) : 1.0f;
                
                if (_soundInstance == nullptr)
                {
                    _soundInstance = _soundManager->PlayLoopingPositionalSound(_sound, SoundPriority::High, Vector2f::Zero, _minDist, _maxDist, 0.0f);
                }
                
                _soundInstance->SetVolume(volume);

                const Vector2f& cameraPos = camera.GetPosition();

                Vector2f closestPos = _path[0];
                float closestDist = std::numeric_limits<float>::max();
                for (uint32_t i = 0; i + 1 < _path.Size(); i++)
                {
                    const Vector2f& a = _path[i];
                    const Vector2f& b = _path[i + 1];

                    Vector2f pt;
                    float dist = Math::PointToLineDistanceSquared(a, b, cameraPos, pt);
                    if (dist < closestDist)
                    {
                        closestDist = dist;
                        closestPos = pt;
                    }
                }

                _soundInstance->SetPosition(closestPos);
            }
        }
    }

    template <>
    void EnumeratePreloads<Audio::LavaSound>(PreloadSet& preloads)
    {
        preloads.insert(Audio::LavaSoundPath);
    }
}
