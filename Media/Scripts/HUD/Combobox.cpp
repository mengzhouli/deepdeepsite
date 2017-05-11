#include "HUD/Combobox.hpp"

namespace Dwarf
{
    namespace HUD
    {

        Combobox::Combobox()
        {

        }

        Combobox::~Combobox()
        {
            SafeRelease(_panel);
            SafeRelease(_highlightPanel);
            SafeRelease(_itemPanel);
            SafeReleaseAndClearContainer(_itemHighlightPanels);

            SafeRelease(_background);

            SafeRelease(_font);

            SafeRelease(_border);
            SafeRelease(_dropDownBorder);
            SafeRelease(_downButton);
            SafeRelease(_highlight);
        }

        void Combobox::SetClickBind(Input::InputBindCode clickBind)
        {
            _clickBind = clickBind;
        }

        void Combobox::SetFont(const Graphics::Font* font)
        {
            if (_font != font)
            {
                SafeRelease(_font);
                _font = font;
                SafeAddRef(_font);

                rebuildPanels();
            }
        }

        void Combobox::SetTextColor(const Color& color)
        {
            if (_textColor != color)
            {
                _textColor = color;
                rebuildPanels();
            }
        }

        void Combobox::SetTextSize(uint32_t size)
        {
            if (_fontSize != size)
            {
                _fontSize = size;
                rebuildPanels();
            }
        }

        void Combobox::SetBackgroundColor(const Color& color)
        {
            _backgroundColor = color;
        }

        void Combobox::SetHighlightMaterial(const PanelDrawable* highlight)
        {
            if (_highlight != highlight)
            {
                SafeRelease(_highlight);
                _highlight = highlight;
                SafeAddRef(_highlight);

                rebuildPanels();
            }
        }

        void Combobox::SetDownButtonMaterial(const PanelDrawable* downButton)
        {
            if (_downButton != downButton)
            {
                SafeRelease(_downButton);
                _downButton = downButton;
                SafeAddRef(_downButton);

                rebuildPanels();
            }
        }

        void Combobox::SetBorder(const BorderPanelDrawable *drawable)
        {
            if (_border != drawable)
            {
                SafeRelease(_border);
                _border = drawable;
                SafeAddRef(_border);

                rebuildPanels();
            }
        }

        void Combobox::SetDropDownBorder(const BorderPanelDrawable *drawable)
        {
            if (_dropDownBorder != drawable)
            {
                SafeRelease(_dropDownBorder);
                _dropDownBorder = drawable;
                SafeAddRef(_dropDownBorder);

                rebuildPanels();
            }
        }

        void Combobox::SetBounds(const Rectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        void Combobox::SetBounds(const RotatedRectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        const RotatedRectanglef& Combobox::GetBounds() const
        {
            return _bounds;
        }

        float Combobox::GetScale() const
        {
            return _scale;
        }

        void Combobox::SetEnabled(bool enabled)
        {
            _enabled = enabled;
        }

        void Combobox::SetVisible(bool visible)
        {
            _hidden = !visible;
        }

        void Combobox::SetItems(const std::vector<std::string>& items)
        {
            _items = items;
            _selectedItem = -1;
            rebuildPanels();
        }

        const std::vector<std::string>& Combobox::GetItems() const
        {
            return _items;
        }

        int32_t Combobox::GetSelection() const
        {
            return _selectedItem;
        }

        void Combobox::SetSelection(int32_t selection)
        {
            int32_t newSelection = Clamp(selection, -1, static_cast<int32_t>(_items.size()) - 1);
            if (_selectedItem != newSelection)
            {
                _selectedItem = newSelection;
                rebuildPanels();
            }
        }

        bool Combobox::IsJustChanged() const
        {
            return _justChanged;
        }

        bool Combobox::IsShowing()
        {
            return !_hidden;
        }

        bool Combobox::IsHighlighted()
        {
            return _mouseOver && _enabled && _highlight != nullptr;
        }

        bool Combobox::IsExpaneded()
        {
            return _expanded;
        }

        void Combobox::Update(const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);
            _justChanged = false;
            _mouseOverItem = -1;
            _mouseOver = false;

            if (!_hidden)
            {
                if (_expanded)
                {
                    if (_enabled && input.IsBindJustPressed(_clickBind))
                    {
                        _selectedItem = -1;
                        _expanded = false;

                        for (uint32_t i = 0; i < _itemHighlightPanels.size(); i++)
                        {
                            RotatedRectanglef itemBounds = _itemPanel->GetChildPanelArea(_itemHighlightPanels[i], _itemPanelBounds, _scale);
                            if (RotatedRectanglef::Contains(itemBounds, mousePos))
                            {
                                _selectedItem = i;
                                _justChanged = true;
                                rebuildPanels();
                                break;
                            }
                        }
                    }

                    _mouseOver = true;
                }
                else if (RotatedRectanglef::Contains(_bounds, mousePos))
                {
                    if (_enabled && input.IsBindJustPressed(_clickBind))
                    {
                        _expanded = !_expanded;
                    }

                    _mouseOver = true;
                }
            }

            updatePanels();
        }

        void Combobox::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const
        {
            _panel->Draw(spriteRenderer, color, _bounds, _scale);
            _itemPanel->Draw(spriteRenderer, color, _itemPanelBounds, _scale);
        }

        void Combobox::rebuildPanels()
        {
            SafeRelease(_panel);
            SafeRelease(_highlightPanel);
            SafeRelease(_itemPanel);
            SafeReleaseAndClearContainer(_itemHighlightPanels);

            auto formatItem = [this](const std::string& item)
            {
                // Todo: Use _textColor
                return Format("<size=%u><color=%s>%s</color></size>", _fontSize, "white", item.c_str());
            };

            _panel = new Panel();
            if (_border)
            {
                _panel->AddAlignedPanelDrawable(Alignment_BackgroundFill, _border);
                _panel->SetVariablePadding(_border->GetPaddingFunction());
            }

            Panel* panelChild = new Panel();
            if (_background)
            {
                panelChild->AddAlignedPanelDrawable(Alignment_Fill, _background);
            }

            if (_font && _selectedItem >= 0)
            {
                panelChild->AddAlignedText(Alignment_Left, _font, formatItem(_items[_selectedItem]));
            }

            if (_downButton)
            {
                panelChild->AddAlignedPanelDrawable(Alignment_Right, _downButton);
            }

            if (_highlight)
            {
                _highlightPanel = new Panel();
                //_highlightPanel->AddAlignedPanelDrawable(Alignment_Fill, _highlight);
                panelChild->AddAlignedPanel(Alignment_BackgroundFill, _highlightPanel);
            }

            _panel->AddAlignedPanel(Alignment_Fill, panelChild);
            SafeRelease(panelChild);

            _itemPanel = new Panel();
            if (_dropDownBorder)
            {
                _itemPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, _dropDownBorder);
                _itemPanel->SetVariablePadding(_dropDownBorder->GetPaddingFunction());
            }

            for (const auto& item : _items)
            {
                Panel* itemPanel = new Panel();
                if (_font)
                {
                    itemPanel->AddAlignedText(Alignment_Left, _font, formatItem(item));
                }

                if (_highlight)
                {
                    Panel* itemHighlightPanel = new Panel();
                    //itemHighlightPanel->AddAlignedPanelDrawable(Alignment_Fill, _highlight);
                    itemPanel->AddAlignedPanel(Alignment_BackgroundFill, itemHighlightPanel);
                    _itemHighlightPanels.push_back(itemHighlightPanel);
                }

                _itemPanel->AddAlignedPanel(Alignment_Top, itemPanel);
                SafeRelease(itemPanel);
            }

            updatePanels();
        }

        void Combobox::updatePanels()
        {
            if (_panel)
            {
                _panel->SetBackgroundColor(_backgroundColor);
                _panel->SetVisible(IsShowing());
            }
            if (_highlightPanel)
            {
                _highlightPanel->SetVisible(IsShowing() && IsHighlighted());
            }

            if (_itemPanel)
            {
                _itemPanel->SetVisible(IsShowing() && IsExpaneded());
                for (auto& itemHighlight : _itemHighlightPanels)
                {
                    itemHighlight->SetVisible(false);
                }
                if (_mouseOverItem >= 0)
                {
                    _itemHighlightPanels[_mouseOverItem]->SetVisible(true);
                }
            }

            Vector2f boundsTop = _bounds.TopRight();
            Vector2f boundsBot = _bounds.BottomRight();
            Vector2f downDir = Vector2f::Normalize(boundsBot - boundsTop);
            float itemsHeight = _itemPanel->GetMinimumSize(_scale).Y;
            _itemPanelBounds.Center = _bounds.Center + (downDir * (_bounds.ExtentY + (itemsHeight * 0.5f)));
            _itemPanelBounds.Extents = Vector2f(_bounds.ExtentX, itemsHeight * 0.5f);
            _itemPanelBounds.R = _bounds.R;
        }
    }
}
