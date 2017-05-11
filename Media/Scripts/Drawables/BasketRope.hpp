#pragma once

#include "Graphics/IDrawable.hpp"
#include "Drawables/RopeDrawable.hpp"
#include "Geometry/Spline.hpp"
#include "Content/ContentManager.hpp"
#include "Content/Preload.hpp"
#include "Animation/SkeletonInstance.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class BasketRopeDrawable final : public IDrawable, public IUpdateable
        {
        public:
            BasketRopeDrawable(Content::ContentManager* cm, const Splinef& spline);
            virtual ~BasketRopeDrawable();

            void Update(double totalTime, float dt) override;

            const Rectanglef& GetDrawBounds() const override;
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const override;

        private:
            struct basket
            {
                Animation::SkeletonInstance* skeleton;
                float currentT;
            };
            std::vector<basket> _baskets;

            float _tStep;

            const Splinef _spline;
            Rectanglef _bounds;

            RopeDrawable* _rope;
        };
    }

    template <>
    void EnumeratePreloads<Graphics::BasketRopeDrawable>(PreloadSet& preloads);
}
