#pragma once

#include "Resource.hpp"
#include "Graphics/IDrawable.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Graphics/MaterialSet.hpp"

#include "EmoteTypes.hpp"

#include <map>

namespace Dwarf
{
    namespace Graphics
    {
        class EmoteDisplay : public Resource, public IDrawable
        {
        public:
            EmoteDisplay()
                : _curTime(0.0f)
                , _duration(0.0f)
                , _color(Color::White)
                , _bounds()
                , _curMaterial(NULL)
                , _materials()
            {
            }

            void SetEmoteMaterial(EmoteType emote, const HUDMaterial* material)
            {
                SafeRelease(_materials[emote]);
                _materials[emote] = material;
                SafeAddRef(material);
            }

            void Emote(EmoteType type, float duration)
            {
                auto iter = _materials.find(type);
                if (iter == _materials.end())
                {
                    return;
                }

                _curTime = 0.0f;
                _duration = duration;
                _curMaterial = iter->second;
                SafeAddRef(_curMaterial);
            }

            void Update(const Rectanglef& headBounds, double totalTime, float dt)
            {
                _curTime += dt;

                if (_curTime < _duration)
                {
                    Rectanglef emoteBounds = _curMaterial->GetPixelArea();

                    const float targetWidth = headBounds.W * 0.75f;
                    const float heightPad = headBounds.H * 0.05f;
                    const float scale = targetWidth / emoteBounds.W;

                    _bounds.Position = Vector2f(headBounds.X + (headBounds.W * 0.5f) - (targetWidth * 0.5f),
                                                headBounds.Y - heightPad - (emoteBounds.H * scale));
                    _bounds.Size = emoteBounds.Size * scale;

                    const float fadeInTime = 0.15f;
                    const float fadeOutTime = 0.25f;

                    float alpha = 1.0f;
                    if (_curTime < fadeInTime)
                    {
                        alpha = SmoothStep(0.0f, 1.0f, _curTime / fadeInTime);
                    }
                    else if (_curTime > (_duration - fadeOutTime))
                    {
                        alpha = SmoothStep(0.0f, 1.0f, (_duration - _curTime) / fadeInTime);
                    }
                    _color.A = uint8_t(Clamp(alpha, 0.0f, 1.0f) * 255);
                }
                else if (_curMaterial)
                {
                    // Reset
                    SafeRelease(_curMaterial);
                    _curTime = 0.0f;
                    _duration = 0.0f;
                }
            }

            const Rectanglef& GetDrawBounds() const
            {
                return _bounds;
            }

            void Draw(SpriteRenderer* spriteRenderer) const
            {
                if (_curMaterial)
                {
                    spriteRenderer->DrawSprite(_curMaterial->GetTexture(), _bounds, _curMaterial->GetSubRect(), _color);
                }
            }

            void Draw(Graphics::LevelRenderer* levelRenderer) const
            {
                levelRenderer->AddDrawable(this, false);
            }

        protected:
            virtual ~EmoteDisplay()
            {
                SafeRelease(_curMaterial);

                for (EmoteMaterialMap::iterator i = _materials.begin(); i != _materials.end(); i++)
                {
                    SafeRelease(i->second);
                }
                _materials.clear();
            }

        private:
            float _curTime;
            float _duration;

            Color _color;
            Rectanglef _bounds;
            const HUDMaterial* _curMaterial;

            typedef std::map<EmoteType, const HUDMaterial*> EmoteMaterialMap;
            EmoteMaterialMap _materials;

        };
    }
}
