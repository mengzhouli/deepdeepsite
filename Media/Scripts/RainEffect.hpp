#pragma once

#include "Vector2.hpp"
#include "Graphics/IDrawable.hpp"
#include "Content/IHasContent.hpp"
#include "Content/Preload.hpp"
#include "IUpdateable.hpp"
#include "Lights/PolygonLight.hpp"
#include "Audio/SoundManager.hpp"
#include "SoundSet.hpp"
#include "TypedEnums.hpp"

namespace Dwarf
{
    namespace Level
    {
        class LevelLayerInstance;
    }

    BETTER_ENUM(RainEnvironment, uint8_t, Muffled, Open);

    class RainEffect final : public IUpdateable, public Content::IHasContent, public Graphics::IDrawable
    {
    public:
        RainEffect(const Level::LevelLayerInstance* layer, Audio::SoundManager* soundManager);
        ~RainEffect();

        const Rotatorf& GetRainDirection() const;
        void SetRainDirection(const Rotatorf& dir);

        float GetRainMinSpeed() const;
        float GetRainMaxSpeed() const;
        void SetRainSpeed(float minSpeed, float maxSpeed);

        uint32_t GetDropCount() const;
        void SetDropCount(uint32_t dropCount);

        const Color& GetDropColor() const;
        void SetDropColor(const Color& color);

        float GetDropMinTransparency() const;
        float GetDropMaxTransparency() const;
        void SetDropTransparency(float minTransparency, float maxTransparency);

        float GetDropMinSize() const;
        float GetDropMaxSize() const;
        void SetDropSize(float minSize, float maxSize);

        bool IsLightningEnabled() const;
        void SetLightningEnabled(bool enabled);

        float GetLightningStrikeMinInterval() const;
        float GetLightningStrikeMaxInterval() const;
        float GetLightningDoubleStrikeChance() const;
        void SetLightningStrikeInterval(float minInterval, float maxInterval, float doubleStrikeChance);

        const Color& GetLightningColor() const;
        void SetLightningColor(const Color& color);

        float GetLightningRampUpTime() const;
        void SetLightingRampUpTime(float rampUpTime);

        float GetLightningFadeTime() const;
        void SetLightingFadeTime(float fadeTime);

        float GetLightningSoundDelay() const;
        void SetLightningSoundDelay(float delay);

        void SetCurrentRainEnvironment(RainEnvironment env);

        void LoadContent(Content::ContentManager* contentManager) override;
        void UnloadContent() override;

        void Update(double totalTime, float dt) override;

        const Lights::PolygonLight& GetLight() const;

        const Rectanglef& GetDrawBounds() const override;
        void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;

    private:
        const Level::LevelLayerInstance* _layer;
        Audio::SoundManager* _soundManager;

        struct Drop
        {
            Vector2f Position;
            float SpeedT;
            float AlphaT;
            float SizeT;
        };

        struct RainGrid
        {
            std::vector<Drop> Drops;
            uint32_t GridSize = 0;
        };
        std::pair<float, float> _rainSpeed = { 1000.0f, 1000.0f };
        Color _dropColor = Color::FromFloats(0.8f, 0.8f, 0.8f, 1.0f);
        std::pair<float, float> _dropTransparency = { 0.25f, 0.75f };
        std::pair<float, float> _dropSize = { 32.0f, 92.0f };
        Rotatorf _rainDirection = Rotatorf::Zero;

        std::vector<RainGrid> _grids;
        const Graphics::Texture* _dropTexture = nullptr;

        struct RainSound
        {
            Audio::SoundSet Sounds;
            std::shared_ptr<Audio::ManagedSoundInstance> PlayingSound;
        };
        std::map<RainEnvironment, RainSound> _rainSounds;
        RainEnvironment _curEnvironment = RainEnvironment::Muffled;

        bool _lightningEnabled = false;
        std::pair<float, float> _lightningInterval = { 10.0f, 25.0f };
        float _lightningDoubleStikeChance = 0.1f;
        Color _lightningColor = Color::White;
        float _lightningRampUpTime = 0.05f;
        float _lightningFadeTime = 0.7f;
        float _lightingSoundDelay = 0.4f;
        float _lightingSoundTimer = std::numeric_limits<float>::max();
        Audio::SoundSet _lightningSounds;

        float _lightningTimer = 0.0f;
        Lights::PolygonLight _light;
    };

    template <>
    void EnumeratePreloads<RainEffect>(PreloadSet& preloads);
}
