#pragma once

#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Geometry/Rectangle.hpp"
#include "Camera.hpp"
#include "HUD/Panel.hpp"
#include "HUD/PanelBorder.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class Checkbox
        {
        public:
            Checkbox();
            ~Checkbox();

            void SetClickBind(Input::InputBindCode clickBind);

            void SetBackgroundColor(const Color& color);
            void SetHighlightMaterial(const PanelDrawable* highlight);
            void SetBorder(const BorderPanelDrawable *drawable);
            void SetCheckMaterial(const PanelDrawable *check);

            void SetBounds(const Rectanglef& bounds, float scale);
            void SetBounds(const RotatedRectanglef& bounds, float scale);
            const RotatedRectanglef& GetBounds() const;
            float GetScale() const;

            void SetEnabled(bool enabled);
            void SetVisible(bool visible);

            bool IsChecked() const;
            void SetChecked(bool checked);
            bool IsJustChanged() const;

            bool IsShowing();
            bool IsHighlighted();

            void Update(const Input::FrameInput& input, const Camera& camera);

            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const;

        private:
            void rebuildPanels();

            void updatePanels();

            Input::InputBindCode _clickBind = Input::InputBindCode_Invalid;

            RotatedRectanglef _bounds = RotatedRectanglef();
            float _scale = 1.0f;

            Panel* _panel = nullptr;
            Panel* _checkPanel = nullptr;
            Panel* _highlightPanel = nullptr;

            bool _checked = false;
            bool _justChanged = false;

            bool _hidden = false;
            bool _enabled = true;

            bool _mouseOver = false;
            bool _mouseJustClicked = false;
            bool _mouseClicking = false;

            Color _backgroundColor = Color::Black;
            const PanelDrawable* _background = nullptr;

            const BorderPanelDrawable* _border = nullptr;
            const PanelDrawable* _highlight = nullptr;
            const PanelDrawable* _check = nullptr;
        };
    }
}
