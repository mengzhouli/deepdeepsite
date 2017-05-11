#pragma once

#include "HUD/Button.hpp"

#include <vector>
#include <assert.h>

namespace Dwarf
{
    namespace HUD
    {
        class ButtonPanel : public Resource
        {
        public:
            ButtonPanel();

            void SetClickBind(Input::InputBindCode clickBind);

            void SetBounds(const RotatedRectanglef& bounds, float scale);
            void SetBounds(const Rectanglef& bounds, float scale);

            const RotatedRectanglef& GetBounds() const;
            const RotatedRectanglef& GetButtonBounds(uint32_t buttonIdx) const;

            void SetHighlightMaterial(const PanelDrawable* highlight);
            void SetBorderMaterial(const BorderPanelDrawable* border);
            void SetCornerBorderMaterial(const BorderPanelDrawable* cornerBorder);
            void SetScrollMaterials(const PanelDrawable* upButton, const PanelDrawable* downButton, const PanelDrawable* dragButton);

            void SetButtonCount(uint32_t count, uint32_t buttonsPerRow);

            void SetButtonIcon(uint32_t index, const PanelDrawable* drawable);
            void SetButtonBackground(uint32_t index, const PanelDrawable* background);
            void SetButtonCornerText(uint32_t index, Alignment alignment, const Graphics::Font* font, const std::string& text);

            void ForceButtonHighlightOn(uint32_t index, bool force);
            void ForceButtonHighlightOff(uint32_t index, bool force);

            void SetButtonIconColor(uint32_t index, const Color& color);

            void SetButtonVisible(uint32_t index, bool visible);
            void SetButtonEnabled(uint32_t index, bool enabled);

            bool IsButtonHighlighted(uint32_t& outButton);
            bool IsButtonClicked(uint32_t& outButton);
            bool IsButtonJustClicked(uint32_t& outButton);
            bool IsButtonMouseOver(uint32_t& outButton);

            void Update(const Input::FrameInput& input, const Camera& camera);

            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const;

        protected:
            virtual ~ButtonPanel();

        private:
            bool usingScrollBar() const;

            void updatePlacement();

            Input::InputBindCode _clickBind;
            Vector2f _prevMousePosition;

            ResourcePointer<Button> _upButton;
            ResourcePointer<Button> _downButton;
            ResourcePointer<Button> _dragButton;

            ResourcePointer<const BorderPanelDrawable> _buttonBorder;
            ResourcePointer<const BorderPanelDrawable> _buttonCornerBorder;
            ResourcePointer<const PanelDrawable> _buttonHighlight;

            RotatedRectanglef _bounds;
            float _scale;

            bool _useScissor;
            Rectanglei _scissor;

            float _scrollPosition;
            uint32_t _rowSize;
            std::vector<ResourcePointer<Button>> _buttons;
            std::vector<bool> _buttonVisibility;
        };
    }
}
