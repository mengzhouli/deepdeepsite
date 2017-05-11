#include "Drawables/Flame.hpp"

#include "Random.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        static const std::string FlameParticlePath = "Particles/torch_1.partsys";

        static const Audio::SoundPathVector FlameStartSoundPath =
        {
            { "Audio/Characters/Brazier/Brazier_Ignite.ogg", 1.0f },
        };
        static const Audio::SoundPathVector FlameBurnSoundPath =
        {
            { "Audio/Characters/Brazier/Brazier_Burning.ogg", 1.0f },
        };

        static const float FlameStartVolume = 0.5f;
        static const float FlameBurningVolume = 0.03f;

        static const float DefaultRadius = 1500.0f;
        static const float DefaultShadowPerc = 0.7f;

        static const float StartSoundFalloffMinDistanceMultiplier = 2.5f;
        static const float StartSoundFalloffMaxDistanceMultiplier = 5.0f;

        static const float LoopingSoundFalloffMinDistanceMultiplier = 0.5f;
        static const float LoopingSoundFalloffMaxDistanceMultiplier = 3.0f;

        Flame::Flame(const Color& color, bool castsShadows, bool startOn, bool showParticles, bool playSounds, Audio::SoundManager* soundManager, const std::string& particlesPath)
            : _lightTimer(Random::RandomBetween(0.0f, 1.0f))

            , _lightOn(startOn)
            , _transitionTimer(0.0f)
            , _turnOnTime(0.25f)
            , _turnOffTime(0.7f)

            , _minBrightness(0.9f)
            , _maxBrightness(1.0f)

            , _scale(1.0f)
            , _lightRadius(DefaultRadius)

            , _showParticles(showParticles)
            , _customParticleSpawner(false)
            , _particlesPosition(Vector2f::Zero)
            , _partsys(nullptr)
            , _particlesPath(particlesPath.empty() ? FlameParticlePath : particlesPath)

            , _soundManager(soundManager)
            , _playSounds(playSounds)
            , _startSound()
            , _burningSound()

            , _baseColor(color)

            , _centerLight(Vector2f::Zero, DefaultRadius, color, 0.0f)
            , _torchLight(Vector2f::Zero, DefaultRadius, color, castsShadows ? DefaultShadowPerc : 0.0f)

            , _bounds()
        {
            if (_playSounds)
            {
                assert(_soundManager);
                _startSound.AddSounds(FlameStartSoundPath);
                _burningSound.AddSounds(FlameBurnSoundPath);
            }
        }

        Flame::~Flame()
        {
        }

        void Flame::SetState(bool on, bool playStateChangeSound)
        {
            if (on != _lightOn)
            {
                _lightOn = on;
                if (_lightOn)
                {
                    _transitionTimer = _turnOnTime;

                    if (_partsys)
                    {
                        _partsys->Start();
                    }

                    if (playStateChangeSound)
                    {
                        _soundManager->PlaySinglePositionalSound(_startSound.GetNextSound(), Audio::SoundPriority::Medium, _centerLight.Position,
                            _lightRadius * StartSoundFalloffMinDistanceMultiplier, _lightRadius * StartSoundFalloffMaxDistanceMultiplier, FlameStartVolume * _scale);
                    }

                    if (_curBurningSound)
                    {
                        _curBurningSound->Stop(0.5f);
                        _curBurningSound = nullptr;
                    }
                    _curBurningSound = _soundManager->PlayLoopingPositionalSound(_burningSound.GetNextSound(), Audio::SoundPriority::Low, _centerLight.Position, 0.0f, 0.0f, FlameBurningVolume * _scale);

                    updateLight(0.0f);
                }
                else
                {
                    _transitionTimer = _turnOffTime;

                    if (_partsys)
                    {
                        _partsys->Stop();
                    }

                    updateLight(0.0f);
                }
            }
        }

        bool Flame::IsOn() const
        {
            return _lightOn;
        }

        void Flame::SetPosition(const Vector2f& pos)
        {
            _torchLight.Position = pos;
            _centerLight.Position = pos;
        }

        void Flame::SetScale(float scale)
        {
            _scale = scale;
        }

        void Flame::SetRadius(float radius)
        {
            _lightRadius = radius;
        }

        float Flame::GetRadius() const
        {
            return _scale * _lightRadius;
        }

        void Flame::SetBrightnessRange(float minBrightness, float maxBrightness)
        {
            _minBrightness = minBrightness;
            _maxBrightness = maxBrightness;
        }

        void Flame::SetCustomParticlePolygonSpawners(const std::vector<Polygonf>& polys)
        {
            if (_partsys)
            {
                _customParticleSpawner = true;
                _partsys->SetPolygonSpawners(polys);
            }
        }

        void Flame::SetCustomParticleFunctionSpawner(Particles::ParticleSpawnFunction func, Particles::ParticleBoundsFunction bounds)
        {
            if (_partsys)
            {
                _customParticleSpawner = true;
                _partsys->SetFunctionSpawner(func, bounds);
            }
        }

        uint32_t Flame::GetParticleCount() const
        {
            return _partsys ? _partsys->GetParticleCount() : 0;
        }

        void Flame::LoadContent(Content::ContentManager* contentManager)
        {
            if (_showParticles)
            {
                _partsys = Content::CreateParticleSystemInstance(contentManager, _particlesPath);
                if (_lightOn)
                {
                    _partsys->Start();
                }
            }

            _startSound.LoadContent(contentManager);
            _burningSound.LoadContent(contentManager);

            if (_playSounds)
            {
                if (_lightOn)
                {
                    _curBurningSound =
                        _soundManager->PlayLoopingPositionalSound(_burningSound.GetNextSound(), Audio::SoundPriority::Low,
                                                                  _centerLight.Position, 0.0f, 0.0f, FlameBurningVolume * _scale);
                }
                updateSounds();
            }
        }

        void Flame::UnloadContent()
        {
            SafeRelease(_partsys);
            _startSound.UnloadContent();
            _burningSound.UnloadContent();
        }

        const Lights::PointLight& Flame::GetCenterLight() const
        {
            return _centerLight;
        }

        const Lights::PointLight& Flame::GetMainLight() const
        {
            return _torchLight;
        }

        bool Flame::ShouldDrawLights() const
        {
            return _lightOn || _transitionTimer > 0.0f;
        }

        void Flame::Update(double totalTime, float dt)
        {
            _transitionTimer -= dt;
            updateLight(dt);
            updateSounds();

            if (_partsys)
            {
                if (!_customParticleSpawner && _particlesPosition != _torchLight.Position)
                {
                    _particlesPosition = _torchLight.Position;
                    _partsys->SetPointSpawner(_particlesPosition);
                }

                _partsys->SetScale(_scale);
                _partsys->SetRotation(0.0f);
                _partsys->Update(totalTime, dt);

                _bounds = _partsys->GetDrawBounds();
            }
            else
            {
                _bounds.Position = _centerLight.Position;
                _bounds.Size = Vector2f::Zero;
            }

            if (_curBurningSound)
            {
                _curBurningSound->SetVolume(FlameBurningVolume * _scale);
            }
        }

        const Rectanglef& Flame::GetDrawBounds() const
        {
            return GetDrawBoundsExtended(ExtendedDrawParameters());
        }

        void Flame::Draw(Graphics::SpriteRenderer* spriteRenderer) const
        {
            DrawExtended(spriteRenderer, ExtendedDrawParameters());
        }

        void Flame::updateLight(float dt)
        {
            _lightTimer += dt;

            const float minGreen = 0.95f;
            const float maxGreen = 1.05f;
            const float colorFreqFast = 2.0f;
            const float colorFreqSlow = 1.25f;

            const float greenPerc = (Sine(_lightTimer * colorFreqFast) + Sine(_lightTimer * colorFreqSlow) + 2.0f) * 0.25f;
            const float green = Lerp(minGreen, maxGreen, greenPerc);

            const float brightnessFreqFast = 6.0f;
            const float brightnessFreqSlow = 2.5f;

            const float brightnessPerc = (Sine(_lightTimer * brightnessFreqFast) + Sine(_lightTimer * brightnessFreqSlow) + 2.0f) * 0.25f;
            float brightness = Lerp(_minBrightness, _maxBrightness, brightnessPerc);

            if (_transitionTimer > 0.0f)
            {
                if (_lightOn)
                {
                    brightness *= 1.0f - (_transitionTimer / _turnOnTime);
                }
                else
                {
                    brightness *= (_transitionTimer / _turnOffTime);
                }
            }

            //_torchLight.LightColor = _baseColor * brightness;
            //_torchLight.LightColor.A = 255;
            _torchLight.LightColor = Color::FromBytes(_baseColor.R * brightness, _baseColor.G * brightness * green, _baseColor.B * brightness, 1.0f);

            const float minRadius = 0.975f;
            const float maxRadius = 1.0f;
            const float radius = Lerp(minRadius, maxRadius, brightnessPerc) * GetRadius();
            _torchLight.Radius = radius;

            _centerLight.Radius = radius * 0.3f;
            _centerLight.LightColor = Color::White * (brightness * 0.5f);
            _centerLight.LightColor.A = 255;
        }

        void Flame::updateSounds()
        {
            const Vector2f& flamePosition = _centerLight.Position;

            float radius = GetRadius();
            if (_transitionTimer > 0.0f)
            {
                if (_lightOn)
                {
                    radius *= 1.0f - (_transitionTimer / _turnOnTime);
                }
                else
                {
                    radius *= (_transitionTimer / _turnOffTime);
                }
            }

            if (_curBurningSound)
            {
                _curBurningSound->SetPosition(flamePosition);
                _curBurningSound->SetMinMaxDistance(radius * LoopingSoundFalloffMinDistanceMultiplier, radius * LoopingSoundFalloffMaxDistanceMultiplier);

                if (!_lightOn && _transitionTimer <= 0.0f && _curBurningSound->GetStatus() == Audio::AudioStatus_Playing)
                {
                    _curBurningSound->Stop(0.5f);
                    _curBurningSound = nullptr;
                }
            }
        }

        bool Flame::ShouldDrawExtended(const ExtendedDrawParameters& params) const
        {
            if (_partsys)
            {
                return _partsys->ShouldDrawExtended(params);
            }
            else
            {
                return false;
            }
        }

        const Rectanglef& Flame::GetDrawBoundsExtended(const ExtendedDrawParameters& params) const
        {
            return _bounds;
        }

        void Flame::DrawExtended(Graphics::SpriteRenderer* spriteRenderer, const ExtendedDrawParameters& params) const
        {
            if (_partsys)
            {
                _partsys->Draw(spriteRenderer);
            }
        }
    }

    template <>
    void EnumeratePreloads<Graphics::Flame>(PreloadSet& preloads)
    {
        preloads.insert(Graphics::FlameParticlePath);

        EnumerateSoundVectorPreloads(preloads, Graphics::FlameStartSoundPath);
        EnumerateSoundVectorPreloads(preloads, Graphics::FlameBurnSoundPath);
    }
}
