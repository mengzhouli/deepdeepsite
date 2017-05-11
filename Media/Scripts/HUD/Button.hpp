#pragma once

#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Geometry/Rectangle.hpp"
#include "Camera.hpp"
#include "HUD/Panel.hpp"
#include "HUD/Icon.hpp"
#include "HUD/PanelBorder.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class Button : public Resource
        {
        public:
            Button();

            void SetBackgroundColor(const Color& color);
            void SetBackground(const PanelDrawable* background);
            void SetIconColor(const Color& color);
            void SetClickBind(Input::InputBindCode clickBind);
            void SetIcon(const PanelDrawable* drawable);
            void SetHighlightMaterial(const PanelDrawable* highlight);
            void SetBorder(const BorderPanelDrawable *drawable);
            void SetCornerBorder(const BorderPanelDrawable *drawable);
            void SetCornerText(Alignment alignment, const Graphics::Font* font, const std::string& text);

            Vector2f GetIconPixelSize() const;

            void SetBounds(const Rectanglef& bounds, float scale);
            void SetBounds(const RotatedRectanglef& bounds, float scale);

            const RotatedRectanglef& GetBounds() const;
            float GetScale() const;

            void SetEnabled(bool enabled);
            void Enable();
            void Disable();
            bool IsEnabled() const;
            void SetVisible(bool visible);
            void Show();
            void Hide();

            bool IsShowing();
            bool IsHighlighted();
            bool IsClicked();
            bool IsJustClicked();
            bool IsMouseOver();

            void ForceHighlightOn(bool force);
            void ForceHighlightOff(bool force);

            void Update(const Input::FrameInput& input, const Camera& camera);

            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const;

        private:
            virtual ~Button();

            void rebuildPanels();

            void updatePanels();

            Input::InputBindCode _clickBind;
            //InputBindCode _keyBind;

            RotatedRectanglef _bounds;
            float _scale;

            ResourcePointer<Panel> _panel;
            ResourcePointer<Panel> _highlightPanel;
            ResourcePointer<Panel> _iconPanel;

            Color _backgroundColor;
            ResourcePointer<const PanelDrawable> _background;

            Color _iconColor;

            bool _mouseOver;
            bool _mouseJustClicked;
            bool _mouseClicking;

            bool _forceHighlightOn;
            bool _forceHighlightOff;

            bool _hidden;
            bool _enabled;

            struct cornerText
            {
                std::string text = "";
                ResourcePointer<const Graphics::Font> font = nullptr;
            };
            std::map<Alignment, cornerText> _cornerTexts;

            ResourcePointer<const PanelDrawable> _icon;
            ResourcePointer<const BorderPanelDrawable> _border;
            ResourcePointer<const BorderPanelDrawable> _cornerBorder;
            ResourcePointer<const PanelDrawable> _highlight;
        };
    }
}
