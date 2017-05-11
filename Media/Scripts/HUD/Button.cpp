#include "HUD/Button.hpp"

namespace Dwarf
{
    namespace HUD
    {
        Button::Button()
            : _clickBind(Input::InputBindCode_Invalid)

            , _bounds()
            , _scale(1.0f)

            , _panel(nullptr)
            , _highlightPanel(nullptr)
            , _iconPanel(nullptr)

            , _backgroundColor(Color::Black)
            , _background(nullptr)

            , _iconColor(Color::White)

            , _mouseOver(false)
            , _mouseJustClicked(false)
            , _mouseClicking(false)

            , _forceHighlightOn(false)
            , _forceHighlightOff(false)

            , _hidden(false)
            , _enabled(true)

            , _icon(nullptr)
            , _border(nullptr)
            , _cornerBorder(nullptr)
            , _highlight(nullptr)
        {
        }

        void Button::SetBackgroundColor(const Color& color)
        {
            _backgroundColor = color;
        }

        void Button::SetBackground(const PanelDrawable* background)
        {
            if (_background != background)
            {
                _background = background;
                rebuildPanels();
            }
        }

        void Button::SetIconColor(const Color& color)
        {
            _iconColor = color;
        }

        void Button::SetClickBind(Input::InputBindCode clickBind)
        {
            _clickBind = clickBind;
        }

        void Button::SetIcon(const PanelDrawable* drawable)
        {
            if (_icon != drawable)
            {
                _icon = drawable;
                rebuildPanels();
            }
        }

        void Button::SetHighlightMaterial(const PanelDrawable* highlight)
        {
            if (_highlight != highlight)
            {
                _highlight = highlight;
                rebuildPanels();
            }
        }

        void Button::SetBorder(const BorderPanelDrawable *drawable)
        {
            if (_border != drawable)
            {
                _border = drawable;
                rebuildPanels();
            }
        }

        void Button::SetCornerBorder(const BorderPanelDrawable *drawable)
        {
            if (_cornerBorder != drawable)
            {
                _cornerBorder = drawable;
                rebuildPanels();
            }
        }

        void Button::SetCornerText(Alignment alignment, const Graphics::Font* font, const std::string& text)
        {
            auto& cornerText = _cornerTexts[alignment];
            if (text != cornerText.text || font != cornerText.font)
            {
                cornerText.text = text;
                cornerText.font = font;
                rebuildPanels();
            }
        }

        Vector2f Button::GetIconPixelSize() const
        {
            if (_icon != nullptr)
            {
                return _icon->GetSize(_scale);
            }
            else
            {
                return Vector2f::Zero;
            }
        }

        void Button::SetBounds(const RotatedRectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        void Button::SetBounds(const Rectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        const RotatedRectanglef& Button::GetBounds() const
        {
            return _bounds;
        }

        float Button::GetScale() const
        {
            return _scale;
        }

        void Button::SetEnabled(bool enabled)
        {
            enabled ? Enable() : Disable();
        }

        void Button::Enable()
        {
            _enabled = true;
        }

        void Button::Disable()
        {
            _enabled = false;
        }

        bool Button::IsEnabled() const
        {
            return _enabled;
        }

        void Button::SetVisible(bool visible)
        {
            visible ? Show() : Hide();
        }

        void Button::Show()
        {
            _hidden = false;
        }

        void Button::Hide()
        {
            _hidden = true;
        }

        bool Button::IsShowing()
        {
            return !_hidden;
        }

        bool Button::IsHighlighted()
        {
            return (_mouseOver || _forceHighlightOn) && !_forceHighlightOff && _enabled && _highlight != nullptr;
        }

        bool Button::IsClicked()
        {
            return _mouseClicking;
        }

        bool Button::IsJustClicked()
        {
            return _mouseJustClicked;
        }

        bool Button::IsMouseOver()
        {
            return _mouseOver;
        }

        void Button::ForceHighlightOn(bool force)
        {
            _forceHighlightOn = force;
        }

        void Button::ForceHighlightOff(bool force)
        {
            _forceHighlightOff = force;
        }

        void Button::Update(const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);

            if (!_hidden)
            {
                _mouseJustClicked = false;
                if (RotatedRectanglef::Contains(_bounds, mousePos))
                {
                    if (_enabled && input.IsBindJustPressed(_clickBind))
                    {
                        _mouseJustClicked = true;
                        _mouseClicking = true;
                    }

                    _mouseOver = true;
                }
                else
                {
                    _mouseOver = false;
                }

                if (input.IsBindReleased(_clickBind))
                {
                    _mouseClicking = false;
                }
            }
            else
            {
                _mouseJustClicked = false;
                _mouseOver = false;
                _mouseClicking = true;
            }

            updatePanels();
        }

        void Button::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const
        {
            _panel->Draw(spriteRenderer, color, _bounds, _scale);
        }

        Button::~Button()
        {
        }

        void Button::rebuildPanels()
        {
            _panel = MakeResource<Panel>();
            if (_border)
            {
                _panel->AddAlignedPanelDrawable(Alignment_BackgroundFill, _border);
                _panel->SetVariablePadding(_border->GetPaddingFunction());
            }

            ResourcePointer<Panel> panelChild = MakeResource<Panel>();
            if (_background)
            {
                panelChild->AddAlignedPanelDrawable(Alignment_Fill, _background);
            }

            _iconPanel = nullptr;
            if (_icon)
            {
                _iconPanel = MakeResource<Panel>();
                _iconPanel->AddAlignedPanelDrawable(Alignment_Fill, _icon);

                panelChild->AddAlignedPanel(Alignment_BackgroundFill, _iconPanel);
            }

            _highlightPanel = nullptr;
            if (_highlight)
            {
                _highlightPanel = MakeResource<Panel>();
                _highlightPanel->AddAlignedPanelDrawable(Alignment_Fill, _highlight);
                panelChild->AddAlignedPanel(Alignment_BackgroundFill, _highlightPanel);
            }

            for (const auto& cornerText : _cornerTexts)
            {
                if (cornerText.second.text.length() > 0 && cornerText.second.font != nullptr)
                {
                    ResourcePointer<Panel> cornerTextPanel = MakeResource<Panel>();
                    if (_cornerBorder)
                    {
                        cornerTextPanel->AddAlignedPanelDrawable(Alignment_BackgroundFill, _cornerBorder);
                        cornerTextPanel->SetVariablePadding(_cornerBorder->GetPaddingFunction());
                    }

                    cornerTextPanel->AddAlignedText(Alignment_Center, cornerText.second.font, cornerText.second.text);

                    panelChild->AddFixedPanel(cornerText.first, Vector2f::Zero, cornerTextPanel);
                }
            }

            _panel->AddAlignedPanel(Alignment_Fill, panelChild);

            updatePanels();
        }

        void Button::updatePanels()
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
            if (_iconPanel)
            {
                _iconPanel->SetColor(_iconColor);
            }
        }

    }
}
