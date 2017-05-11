#include "HUD/Checkbox.hpp"

namespace Dwarf
{
    namespace HUD
    {

        Checkbox::Checkbox()
        {

        }

        Checkbox::~Checkbox()
        {
            SafeRelease(_panel);
            SafeRelease(_highlightPanel);
            SafeRelease(_checkPanel);

            SafeRelease(_background);

            SafeRelease(_border);
            SafeRelease(_highlight);
            SafeRelease(_check);
        }

        void Checkbox::SetClickBind(Input::InputBindCode clickBind)
        {
            _clickBind = clickBind;
        }

        void Checkbox::SetBackgroundColor(const Color& color)
        {
            _backgroundColor = color;
        }

        void Checkbox::SetHighlightMaterial(const PanelDrawable* highlight)
        {
            if (_highlight != highlight)
            {
                SafeRelease(_highlight);
                _highlight = highlight;
                SafeAddRef(_highlight);

                rebuildPanels();
            }
        }

        void Checkbox::SetBorder(const BorderPanelDrawable *drawable)
        {
            if (_border != drawable)
            {
                SafeRelease(_border);
                _border = drawable;
                SafeAddRef(_border);

                rebuildPanels();
            }
        }

        void Checkbox::SetCheckMaterial(const PanelDrawable *check)
        {
            if (_check != check)
            {
                SafeRelease(_check);
                _check = check;
                SafeAddRef(_check);

                rebuildPanels();
            }
        }

        void Checkbox::SetBounds(const RotatedRectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        void Checkbox::SetBounds(const Rectanglef& bounds, float scale)
        {
            _bounds = bounds;
            _scale = scale;
        }

        const Dwarf::RotatedRectanglef& Checkbox::GetBounds() const
        {
            return _bounds;
        }

        float Checkbox::GetScale() const
        {
            return _scale;
        }

        void Checkbox::SetEnabled(bool enabled)
        {
            _enabled = enabled;
        }

        void Checkbox::SetVisible(bool visible)
        {
            _hidden = !visible;
        }

        bool Checkbox::IsChecked() const
        {
            return _checked;
        }

        void Checkbox::SetChecked(bool checked)
        {
            _checked = checked;
        }

        bool Checkbox::IsJustChanged() const
        {
            return _justChanged;
        }

        bool Checkbox::IsShowing()
        {
            return !_hidden;
        }

        bool Checkbox::IsHighlighted()
        {
            return _mouseOver && _enabled && _highlight != nullptr;
        }

        void Checkbox::Update(const Input::FrameInput& input, const Camera& camera)
        {
            Vector2f mousePos = input.UnProjectMousePosition(camera);
            _justChanged = false;

            if (!_hidden)
            {
                _mouseJustClicked = false;
                if (RotatedRectanglef::Contains(_bounds, mousePos))
                {
                    if (_enabled && input.IsBindJustPressed(_clickBind))
                    {
                        _mouseJustClicked = true;
                        _mouseClicking = true;
                        _checked = !_checked;
                        _justChanged = true;
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

        void Checkbox::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const
        {
            _panel->Draw(spriteRenderer, color, _bounds, _scale);
        }

        void Checkbox::rebuildPanels()
        {
            SafeRelease(_panel);
            SafeRelease(_checkPanel);
            SafeRelease(_highlightPanel);

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

            if (_check)
            {
                _checkPanel = new Panel();
                _checkPanel->AddAlignedPanelDrawable(Alignment_Fill, _check);

                panelChild->AddAlignedPanel(Alignment_BackgroundFill, _checkPanel);
            }

            if (_highlight)
            {
                _highlightPanel = new Panel();
                _highlightPanel->AddAlignedPanelDrawable(Alignment_Fill, _highlight);
                panelChild->AddAlignedPanel(Alignment_BackgroundFill, _highlightPanel);
            }

            _panel->AddAlignedPanel(Alignment_Fill, panelChild);
            SafeRelease(panelChild);

            updatePanels();
        }

        void Checkbox::updatePanels()
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
            if (_checkPanel)
            {
                _checkPanel->SetVisible(IsShowing() && IsChecked());
            }
        }

    }
}
