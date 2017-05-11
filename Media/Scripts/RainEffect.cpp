#include "RainEffect.hpp"
#include "level/LevelInstance.hpp"
#include "level/LevelLayerInstance.hpp"
#include "Random.hpp"
#include "Utility.hpp"
#include "Graphics/GraphicsUtility.hpp"

#include <limits>
#include <assert.h>

namespace Dwarf
{
    static const std::string RainDropTexture = "Particles/raindrop.png";
    static const std::vector<uint32_t> RainGridSizes
    {
        2048 * 4,
        2048 * 6,
        2048 * 8,
        2048 * 10,
    };

    static const std::map<RainEnvironment, Audio::SoundPathVector> RainSounds =
    {
        {
            RainEnvironment::Muffled,
            {
                { "Audio/Ambiance/Ambience_Rain1_indoorsMetalRoof_Stereo.ogg", 1.0f },
            }
        },
        {
            RainEnvironment::Open,
            {
                //{ "Audio/Ambiance/Ambience_Rain2_soft_Stereo.ogg", 1.0f },
                //{ "Audio/Ambiance/Ambience_Rain3_medium_Stereo.ogg", 1.0f },
                { "Audio/Ambiance/Ambience_Rain4_loud_Stereo.ogg", 1.0f },
            }
        },
    };
    static const float RainVolume = 0.5f;
    static const float RainVolumeChangeSpeed = 0.5f;

    static const Audio::SoundPathVector LightningSounds = 
    {
        { "Audio/Ambiance/Ambience_Lightning1_Distant_Stereo.ogg", 1.0f },
        { "Audio/Ambiance/Ambience_Lightning2_DistantLong_Stereo.ogg", 1.0f },
        { "Audio/Ambiance/Ambience_Lightning3_Close_Stereo.ogg", 1.0f },
        { "Audio/Ambiance/Ambience_Lightning4_Close_Stereo.ogg", 1.0f },
        { "Audio/Ambiance/Ambience_Lightning5_CloseLong_Stereo.ogg", 1.0f },
    };
    static const float LightningVolume = 0.75f;

    RainEffect::RainEffect(const Level::LevelLayerInstance* layer, Audio::SoundManager* soundManager)
        : _layer(layer)
        , _soundManager(soundManager)
    {
        assert(_layer);
        SafeAddRef(_layer);

        assert(_soundManager);

        for (auto gridSize : RainGridSizes)
        {
            RainGrid grid;
            grid.GridSize = gridSize;
            _grids.emplace_back(grid);
        }

        _light.Area = _layer->GetLevel()->GetCameraController().GetBounds();
        _light.LightColor = Color::Black;

        for (const auto& rainSound : RainSounds)
        {
            _rainSounds[rainSound.first].Sounds.AddSounds(rainSound.second);
        }

        _lightningSounds.AddSounds(LightningSounds);
    }

    RainEffect::~RainEffect()
    {
        SafeRelease(_layer);
        assert(_dropTexture == nullptr);
    }

    const Rotatorf& RainEffect::GetRainDirection() const
    {
        return _rainDirection;
    }

    void RainEffect::SetRainDirection(const Rotatorf& dir)
    {
        _rainDirection = dir;
    }

    float RainEffect::GetRainMinSpeed() const
    {
        return _rainSpeed.first;
    }

    float RainEffect::GetRainMaxSpeed() const
    {
        return _rainSpeed.second;
    }

    void RainEffect::SetRainSpeed(float minSpeed, float maxSpeed)
    {
        _rainSpeed = std::make_pair(minSpeed, maxSpeed);
    }

    uint32_t RainEffect::GetDropCount() const
    {
        uint32_t count = 0;
        for (auto& grid : _grids)
        {
            count += grid.Drops.size();
        }
        return count;
    }

    void RainEffect::SetDropCount(uint32_t dropCount)
    {
        uint32_t dropsPerGrid = dropCount / _grids.size();
        for (auto& grid : _grids)
        {
            if (grid.Drops.size() > dropsPerGrid)
            {
                grid.Drops.resize(dropsPerGrid);
            }
            while (grid.Drops.size() < dropsPerGrid)
            {
                Drop newDrop;
                newDrop.Position = Vector2f(Random::RandomBetween(0.0f, static_cast<float>(grid.GridSize)),
                                            Random::RandomBetween(0.0f, static_cast<float>(grid.GridSize)));
                newDrop.SpeedT = Random::RandomBetween(0.0f, 1.0f);
                newDrop.AlphaT = Random::RandomBetween(0.0f, 1.0f);
                newDrop.SizeT = Random::RandomBetween(0.0f, 1.0f);

                grid.Drops.push_back(newDrop);
            }
        }
    }

    const Color& RainEffect::GetDropColor() const
    {
        return _dropColor;
    }

    void RainEffect::SetDropColor(const Color& color)
    {
        _dropColor = color;
    }

    float RainEffect::GetDropMinTransparency() const
    {
        return _dropTransparency.first;
    }

    float RainEffect::GetDropMaxTransparency() const
    {
        return _dropTransparency.second;
    }

    void RainEffect::SetDropTransparency(float minTransparency, float maxTransparency)
    {
        _dropTransparency = std::make_pair(minTransparency, maxTransparency);
    }

    float RainEffect::GetDropMinSize() const
    {
        return _dropSize.first;
    }

    float RainEffect::GetDropMaxSize() const
    {
        return _dropSize.second;
    }

    void RainEffect::SetDropSize(float minSize, float maxSize)
    {
        _dropSize = std::make_pair(minSize, maxSize);
    }

    bool RainEffect::IsLightningEnabled() const
    {
        return _lightningEnabled;
    }

    void RainEffect::SetLightningEnabled(bool enabled)
    {
        _lightningEnabled = enabled;
    }

    float RainEffect::GetLightningStrikeMinInterval() const
    {
        return _lightningInterval.first;
    }

    float RainEffect::GetLightningStrikeMaxInterval() const
    {
        return _lightningInterval.second;
    }

    float RainEffect::GetLightningDoubleStrikeChance() const
    {
        return _lightningDoubleStikeChance;
    }

    void RainEffect::SetLightningStrikeInterval(float minInterval, float maxInterval, float doubleStrikeChance)
    {
        _lightningInterval = std::make_pair(minInterval, maxInterval);
        _lightningDoubleStikeChance = doubleStrikeChance;
    }

    const Color& RainEffect::GetLightningColor() const
    {
        return _lightningColor;
    }

    void RainEffect::SetLightningColor(const Color& color)
    {
        _lightningColor = color;
    }

    float RainEffect::GetLightningRampUpTime() const
    {
        return _lightningRampUpTime;
    }

    void RainEffect::SetLightingRampUpTime(float rampUpTime)
    {
        _lightningRampUpTime = rampUpTime;
    }

    float RainEffect::GetLightningFadeTime() const
    {
        return _lightningFadeTime;
    }

    void RainEffect::SetLightingFadeTime(float fadeTime)
    {
        _lightningFadeTime = fadeTime;
    }

    float RainEffect::GetLightningSoundDelay() const
    {
        return _lightingSoundDelay;
    }

    void RainEffect::SetLightningSoundDelay(float delay)
    {
        _lightingSoundDelay = delay;
    }

    void RainEffect::SetCurrentRainEnvironment(RainEnvironment env)
    {
        _curEnvironment = env;
    }

    void RainEffect::LoadContent(Content::ContentManager* contentManager)
    {
        _dropTexture = contentManager->Load<Graphics::Texture>(RainDropTexture);
        _lightningSounds.LoadContent(contentManager);

        for (auto& rainSound : _rainSounds)
        {
            rainSound.second.Sounds.LoadContent(contentManager);
            rainSound.second.PlayingSound = _soundManager->PlayLoopingGlobalSound(rainSound.second.Sounds.GetNextSound(), Audio::SoundPriority::High, 0.0f);
        }
    }

    void RainEffect::UnloadContent()
    {
        SafeRelease(_dropTexture);
        _lightningSounds.UnloadContent();

        for (auto& rainSound : _rainSounds)
        {
            rainSound.second.Sounds.UnloadContent();
            rainSound.second.PlayingSound.reset();
        }
    }

    void RainEffect::Update(double totalTime, float dt)
    {
        for (auto& grid : _grids)
        {
            const Vector2f deltaDir = _rainDirection.ToVector(dt);
            for (auto& drop : grid.Drops)
            {
                Vector2f deltaPos = deltaDir * Lerp(_rainSpeed.first, _rainSpeed.second, drop.SpeedT);
                drop.Position += deltaPos;

                drop.Position.X = Mod<float>(drop.Position.X, grid.GridSize);
                drop.Position.Y = Mod<float>(drop.Position.Y, grid.GridSize);
            }
        }

        if (_lightningEnabled)
        {
            _lightingSoundTimer -= dt;
            if (_lightingSoundTimer < 0.0f)
            {
                _soundManager->PlaySingleGlobalSound(_lightningSounds.GetNextSound(), Audio::SoundPriority::High, LightningVolume);
                _lightingSoundTimer = std::numeric_limits<float>::max();
            }

            _lightningTimer -= dt;
            if (_lightningTimer < 0.0f)
            {
                _lightningTimer = _lightningRampUpTime + _lightningFadeTime + _lightingSoundDelay;
                if (!Random::RandomBool(_lightningDoubleStikeChance))
                {
                    _lightningTimer += Random::RandomBetween(_lightningInterval.first, _lightningInterval.second);
                }
                _lightingSoundTimer = _lightingSoundDelay;
            }

            float lightningBrightness = 0.0f;
            if (_lightningTimer < _lightningFadeTime)
            {
                lightningBrightness = _lightningTimer / _lightningFadeTime;
            }
            else if (_lightningTimer < _lightningRampUpTime + _lightningFadeTime)
            {
                lightningBrightness = 1.0f - ((_lightningTimer - _lightningFadeTime) / _lightningRampUpTime);
            }
            _light.LightColor = Color::Lerp(Color::Black, _lightningColor, lightningBrightness);
        }
        else
        {
            _light.LightColor = Color::Black;
        }

        for (auto& rainSound : _rainSounds)
        {
            bool current = _curEnvironment == rainSound.first;

            float deltaVolume = RainVolumeChangeSpeed * dt * RainVolume;
            if (!current)
            {
                deltaVolume = -deltaVolume;
            }

            float newVolume = Clamp(rainSound.second.PlayingSound->GetVolume() + deltaVolume, 0.0f, RainVolume);
            rainSound.second.PlayingSound->SetVolume(newVolume);
        }
    }

    const Lights::PolygonLight& RainEffect::GetLight() const
    {
        return _light;
    }

    const Rectanglef& RainEffect::GetDrawBounds() const
    {
        static const Rectanglef bounds(Vector2f(std::numeric_limits<float>::lowest() * 0.5f), Vector2f(std::numeric_limits<float>::max()));
        return bounds;
    }

    void RainEffect::Draw(Graphics::SpriteRenderer* spriteRenderer) const
    {
        Camera cam = _layer->GetCamera();
        Rectanglef bounds = cam.GetViewBounds().ToRectangle();

        auto roundUp = [](int32_t numToRound, int32_t multiple)
        {
            int32_t isPositive = static_cast<int32_t>(numToRound >= 0);
            return ((numToRound + isPositive * (multiple - 1)) / multiple) * multiple;
        };

        for (auto& grid : _grids)
        {
            int32_t startX = roundUp(static_cast<int32_t>(bounds.X), grid.GridSize) - static_cast<int32_t>(grid.GridSize);
            int32_t startY = roundUp(static_cast<int32_t>(bounds.Y), grid.GridSize) - static_cast<int32_t>(grid.GridSize);

            const Rotatorf dropRotation = Rotatorf::Reflect(_rainDirection, Rotatorf::PiOver2) + Rotatorf::PiOver2;

            for (int32_t y = startY; y < (bounds.Bottom()); y += grid.GridSize)
            {
                for (int32_t x = startX; x < (bounds.Right()); x += grid.GridSize)
                {
                    Vector2f gridPos(x, y);
                    for (const auto& drop : grid.Drops)
                    {
                        const float dropSize = Lerp(_dropSize.first, _dropSize.second, drop.SizeT);
                        const float dropScale = dropSize / _dropTexture->Height();

                        const uint8_t dropAlpha = Saturate(Lerp(_dropTransparency.first, _dropTransparency.second, drop.AlphaT)) * 255.0f;
                        Color dropColor = _dropColor;
                        dropColor.A = dropAlpha;

                        spriteRenderer->DrawSprite(_dropTexture, gridPos + drop.Position, Rectanglef(0, 0, 1, 1), dropColor, dropRotation, Vector2f::Zero, dropScale);
                    }
                }
            }
        }

        if (_lightningEnabled)
        {
            uint8_t luma = _light.LightColor.GetLuminance();
            if (luma > 0)
            {
                Color c = _light.LightColor;
                c.A = luma;

                Graphics::DrawFilledPolygon(spriteRenderer, _light.Area, c);
            }
        }
    }

    template <>
    void EnumeratePreloads<RainEffect>(PreloadSet& preloads)
    {
        preloads.insert(RainDropTexture);
        for (const auto& rainSound : RainSounds)
        {
            EnumerateSoundVectorPreloads(preloads, rainSound.second);
        }
        EnumerateSoundVectorPreloads(preloads, LightningSounds);
    }
}
