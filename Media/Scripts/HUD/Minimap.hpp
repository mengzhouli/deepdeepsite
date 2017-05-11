#pragma once

#include "NonCopyable.hpp"
#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Graphics/Minimap.hpp"
#include "Geometry/Rectangle.hpp"
#include "Content/Preload.hpp"

#include "Animation/SkeletonInstance.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class Minimap : public NonCopyable
        {
        public:
            Minimap(Graphics::Minimap* minimap, Content::ContentManager* contentManager, Input::InputBindCode clickBind);
            ~Minimap();

            void SetPosition(const Vector2f& pos, float scale);
            const Rectanglef& GetBounds() const;
            float GetScale() const;

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

            bool IsMouseOver() const;
            bool IsClicked(Vector2f& worldPosition) const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            void updateMinimapSize();

            Input::InputBindCode _clickBind;

            Animation::SkeletonInstance* _panel;
            Graphics::Minimap* _minimap;

            bool _mouseOver;
            bool _clicked;
            Vector2f _worldPosition;
        };
    }

    template <>
    void EnumeratePreloads<HUD::Minimap>(PreloadSet& preloads);
}
