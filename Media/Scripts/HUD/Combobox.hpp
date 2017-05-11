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
        class Combobox
        {
        public:
            Combobox();
            ~Combobox();

            void SetClickBind(Input::InputBindCode clickBind);

            void SetFont(const Graphics::Font* font);
            void SetTextColor(const Color& color);
            void SetTextSize(uint32_t size);

            void SetBackgroundColor(const Color& color);
            void SetHighlightMaterial(const PanelDrawable* highlight);
            void SetDownButtonMaterial(const PanelDrawable* downButton);
            void SetBorder(const BorderPanelDrawable *drawable);
            void SetDropDownBorder(const BorderPanelDrawable *drawable);

            void SetBounds(const Rectanglef& bounds, float scale);
            void SetBounds(const RotatedRectanglef& bounds, float scale);
            const RotatedRectanglef& GetBounds() const;
            float GetScale() const;

            void SetEnabled(bool enabled);
            void SetVisible(bool visible);

            void SetItems(const std::vector<std::string>& items);
            const std::vector<std::string>& GetItems() const;

            int32_t GetSelection() const;
            void SetSelection(int32_t selection);
            bool IsJustChanged() const;

            bool IsShowing();
            bool IsHighlighted();
            bool IsExpaneded();

            void Update(const Input::FrameInput& input, const Camera& camera);

            void Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const;

        private:
            void rebuildPanels();
            void updatePanels();

            Input::InputBindCode _clickBind = Input::InputBindCode_Invalid;

            RotatedRectanglef _bounds = RotatedRectanglef();
            float _scale = 1.0f;

            Panel* _panel = nullptr;
            Panel* _highlightPanel = nullptr;
            Panel* _itemPanel = nullptr;
            RotatedRectanglef _itemPanelBounds;
            std::vector<Panel*> _itemHighlightPanels;

            int32_t _selectedItem = -1;
            int32_t _mouseOverItem = -1;
            bool _justChanged = false;
            std::vector<std::string> _items;

            bool _expanded = false;

            bool _hidden = false;
            bool _enabled = true;

            bool _mouseOver = false;

            Color _backgroundColor = Color::Black;
            const PanelDrawable* _background = nullptr;

            const Graphics::Font* _font = nullptr;
            Color _textColor = Color::White;
            uint32_t _fontSize = 16;

            const BorderPanelDrawable* _border = nullptr;
            const BorderPanelDrawable* _dropDownBorder = nullptr;
            const PanelDrawable* _downButton = nullptr;
            const PanelDrawable* _highlight = nullptr;
        };
    }
}
