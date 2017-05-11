#pragma once

#include "Content/IHasContent.hpp"
#include "Graphics/IDrawable.hpp"
#include "Geometry/Rectangle.hpp"
#include "Geometry/Chain.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Content/Preload.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class RopeDrawable;

        class GrappleRopeDrawable : public IDrawable, public Resource
        {
        public:
            GrappleRopeDrawable(Content::ContentManager* cm);

            void SetChain(Chainf chain);
            void SetColor(const Color& color);
            void SetDrawPeg(bool drawPeg);

            const Rectanglef& GetHookBounds() const;

            const Rectanglef& GetDrawBounds() const override;

            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;

        protected:
            virtual ~GrappleRopeDrawable();

        private:
            Animation::SkeletonInstance* _hook;
            Animation::SkeletonInstance* _peg;
            RopeDrawable* _rope;

            bool _drawPeg;

            Rectanglef _bounds;
        };
    }

    template <>
    void EnumeratePreloads<Graphics::GrappleRopeDrawable>(PreloadSet& preloads);
}
