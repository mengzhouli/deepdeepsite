#pragma once

#include "Graphics/IDrawable.hpp"
#include "Geometry/Chain.hpp"
#include "Geometry/Spline.hpp"
#include "Content/ContentManager.hpp"
#include "Content/Preload.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class RopeDrawable : public IDrawable, public Resource
        {
        public:
            RopeDrawable(Content::ContentManager* cm);

            void SetChain(const Chainf& chain);
            void SetSpline(const Splinef& spline, uint32_t numSegments);
            void SetColor(const Color& color);

            const Rectanglef& GetDrawBounds() const override;
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;

            bool ContainsPoint(const Vector2f& pt) const;

        protected:
            virtual ~RopeDrawable();

        private:
            void updateVertices();

            const Graphics::Texture* _texture;
            Color _color;
            Chainf _chain;
            float _width;

            struct vertex
            {
                Vector2f pos;
                Vector2f texcoord;
            };
            std::vector<vertex> _vertices;
        };
    }

    template <>
    void EnumeratePreloads<Graphics::RopeDrawable>(PreloadSet& preloads);
}
