#pragma once

#include "Graphics/IDrawable.hpp"
#include "Graphics/LevelRenderer.hpp"
#include "Resource.hpp"
#include "Curve.hpp"
#include "Random.hpp"

#include <list>

namespace Dwarf
{
    namespace Graphics
    {
        class OverheadTextDisplay : public Resource
        {
        public:
            OverheadTextDisplay()
                : _instances()
            {
                _positionCurve.AddPoint(0.0f, Vector2f(0.0f, 0.0f));
                _positionCurve.AddPoint(0.5f, Vector2f(100.0f, -100.0f));
                _positionCurve.AddPoint(1.0f, Vector2f(200.0f, 0.0f));
                _positionCurve.AddPoint(1.0f, Vector2f(200.0f, 0.0f));

                _alphaCurve.AddPoint(0.0f, 0.0f);
                _alphaCurve.AddPoint(0.1f, 1.0f);
                _alphaCurve.AddPoint(0.5f, 1.0f);
                _alphaCurve.AddPoint(0.9f, 1.0f);
                _alphaCurve.AddPoint(1.0f, 0.0f);
                _alphaCurve.AddPoint(1.0f, 0.0f);

                _scaleCurve.AddPoint(0.0f, 1.0f);
                _scaleCurve.AddPoint(0.1f, 1.4f);
                _scaleCurve.AddPoint(0.4f, 1.0f);
                _scaleCurve.AddPoint(0.8f, 1.0f);
                _scaleCurve.AddPoint(1.0f, 0.0f);
                _scaleCurve.AddPoint(1.0f, 0.0f);
            }

            void AddText(const Graphics::PreparedText& text, const Vector2f& position)
            {
                const float duration = 0.75f;

                _instances.push_back(TextInstance(text, position, Vector2f(Random::RandomBetween(-1.0f, 1.0f), Random::RandomBetween(1.0f, 1.2f)), duration));
                updateInstance(_instances.back(), 0.0f);
            }

            void Update(double totalTime, float dt)
            {
                auto iter = _instances.begin();
                while (iter != _instances.end())
                {
                    auto& instance = *iter;

                    if (instance.time > instance.duration)
                    {
                        iter = _instances.erase(iter);
                        continue;
                    }

                    updateInstance(instance, dt);

                    iter++;
                }
            }

            void Draw(Graphics::LevelRenderer* levelRenderer) const
            {
                for (const auto& instance : _instances)
                {
                    levelRenderer->AddDrawable(&instance, false);
                }
            }

        private:
            struct TextInstance : public IDrawable
            {
                TextInstance()
                    : TextInstance(Graphics::PreparedText(), Vector2f::Zero, 0.0f, 0.0f)
                {
                }

                TextInstance(const Graphics::PreparedText& text, const Vector2f& position, const Vector2f& shift, float duration)
                    : origin(position)
                    , shift(shift)

                    , preparedText(text)

                    , bounds()
                    , alpha(1.0f)

                    , time(0.0f)
                    , duration(duration)
                {
                }

                const Rectanglef& GetDrawBounds() const
                {
                    return bounds;
                }

                void Draw(Graphics::SpriteRenderer* spriteRenderer) const
                {
                    spriteRenderer->DrawString(preparedText, bounds.Position, Color::FromFloats(1.0f, 1.0f, 1.0f, Clamp(alpha, 0.0f, 1.0f)));
                }

                Vector2f origin;
                Vector2f shift;

                PreparedText preparedText;

                Rectanglef bounds;
                float alpha;

                float time;
                const float duration;
            };

            void updateInstance(TextInstance& instance, float dt) const
            {
                instance.time += dt;
                float t = Clamp(instance.time / instance.duration, 0.0f, 1.0f);

                Vector2f curvePos = _positionCurve.Evaulute(t);
                Vector2f position = instance.origin + curvePos * instance.shift;
                Vector2f size = _scaleCurve.Evaulute(t) * Vector2f(instance.preparedText.GetSize());

                instance.bounds = Rectanglef(position - (size * 0.5f), size);
                instance.alpha = _alphaCurve.Evaulute(t);
            }

            Curve2f _positionCurve;
            Curvef _alphaCurve;
            Curvef _scaleCurve;

            std::list<TextInstance> _instances;
            const Graphics::Font* _font;
        };
    }
}
