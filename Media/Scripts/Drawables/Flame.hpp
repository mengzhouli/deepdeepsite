#pragma once

#include "Content/IHasContent.hpp"
#include "Graphics/IDrawable.hpp"
#include "Particles/ParticleSystemInstance.hpp"
#include "Lights/PointLight.hpp"
#include "Content/Preload.hpp"
#include "Audio/SoundManager.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        const Color DefaultFlameColor = Color::FromBytes(255, 185, 130, 255);

        class Flame : public IDrawableExtended, public IUpdateable, public Content::IHasContent, public NonCopyable
        {
        public:
            Flame(const Color& color, bool castsShadows, bool startOn, bool showParticles, bool playSounds, Audio::SoundManager* soundManager,
                  const std::string& particlesPath = "");
            virtual ~Flame();

            void SetState(bool on, bool playStateChangeSound = true);
            bool IsOn() const;

            void SetPosition(const Vector2f& pos);
            void SetScale(float scale);

            void SetRadius(float radius);
            float GetRadius() const;

            void SetBrightnessRange(float minBrightness, float maxBrightness);

            void SetCustomParticlePolygonSpawners(const std::vector<Polygonf>& polys);
            void SetCustomParticleFunctionSpawner(Particles::ParticleSpawnFunction func, Particles::ParticleBoundsFunction bounds);

            uint32_t GetParticleCount() const;

            void LoadContent(Content::ContentManager* contentManager);
            void UnloadContent();

            const Lights::PointLight& GetCenterLight() const;
            const Lights::PointLight& GetMainLight() const;
            bool ShouldDrawLights() const;

            void Update(double totalTime, float dt) override;

            // IDrawable
            const Rectanglef& GetDrawBounds() const override;
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;

            // IDrawableExtended
            virtual bool ShouldDrawExtended(const ExtendedDrawParameters& params) const override;
            virtual const Rectanglef& GetDrawBoundsExtended(const ExtendedDrawParameters& params) const override;
            virtual void DrawExtended(SpriteRenderer* spriteRenderer, const ExtendedDrawParameters& params) const override;

        private:
            void updateSounds();
            void updateLight(float dt);

            float _lightTimer;

            bool _lightOn;
            float _transitionTimer;
            float _turnOnTime;
            float _turnOffTime;

            float _minBrightness;
            float _maxBrightness;

            float _scale;
            float _lightRadius;

            bool _showParticles;
            bool _customParticleSpawner;
            Vector2f _particlesPosition;
            Particles::ParticleSystemInstance* _partsys;
            std::string _particlesPath;

            Audio::SoundManager* _soundManager;
            bool _playSounds;
            Audio::SoundSet _startSound;
            Audio::SoundSet _burningSound;
            std::shared_ptr<Audio::ManagedSoundInstance> _curBurningSound;

            const Color _baseColor;

            Lights::PointLight _centerLight;
            Lights::PointLight _torchLight;

            Rectanglef _bounds;
        };
    }

    template <>
    void EnumeratePreloads<Graphics::Flame>(PreloadSet& preloads);
}
