#include "HUD/ButtonPanel.hpp"

namespace Dwarf
{
    namespace HUD
    {
        ButtonPanel::ButtonPanel()
            : _clickBind(Input::InputBindCode_Invalid)
            , _prevMousePosition()
            , _upButton(MakeResource<Button>())
            , _downButton(MakeResource<Button>())
            , _dragButton(MakeResource<Button>())
            , _buttonBorder(nullptr)
            , _buttonCornerBorder(nullptr)
            , _buttonHighlight(nullptr)
            , _scrollPosition(0.0f)
            , _bounds()
            , _scale(1.0f)
            , _useScissor(false)
            , _scissor()
            , _rowSize(1)
            , _buttons()
        {
        }

        void ButtonPanel::SetClickBind(Input::InputBindCode clickBind)
        {
            _clickBind = clickBind;

            _upButton->SetClickBind(_clickBind);
            _downButton->SetClickBind(_clickBind);
            _dragButton->SetClickBind(_clickBind);
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->SetClickBind(_clickBind);
            }
        }

        void ButtonPanel::SetBounds(const Rectanglef& bounds, float scale)
        {
            SetBounds(RotatedRectanglef(bounds), scale);
        }

        void ButtonPanel::SetBounds(const RotatedRectanglef& bounds, float scale)
        {
            if (_bounds != bounds || _scale != scale)
            {
                _bounds = bounds;
                _scale = scale;
                updatePlacement();
            }
        }

        const RotatedRectanglef& ButtonPanel::GetBounds() const
        {
            return _bounds;
        }

        const RotatedRectanglef& ButtonPanel::GetButtonBounds(uint32_t buttonIdx) const
        {
            return _buttons[buttonIdx]->GetBounds();
        }

        void ButtonPanel::SetHighlightMaterial(const PanelDrawable* highlight)
        {
            _buttonHighlight = highlight;

            _upButton->SetHighlightMaterial(_buttonHighlight);
            _downButton->SetHighlightMaterial(_buttonHighlight);
            _dragButton->SetHighlightMaterial(_buttonHighlight);
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->SetHighlightMaterial(_buttonHighlight);
            }
        }

        void ButtonPanel::SetBorderMaterial(const BorderPanelDrawable* border)
        {
            _buttonBorder = border;

            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->SetBorder(_buttonBorder);
            }
        }

        void ButtonPanel::SetCornerBorderMaterial(const BorderPanelDrawable* cornerBorder)
        {
            _buttonCornerBorder = cornerBorder;

            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->SetCornerBorder(_buttonCornerBorder);
            }
        }

        void ButtonPanel::SetScrollMaterials(const PanelDrawable* upButton, const PanelDrawable* downButton, const PanelDrawable* dragButton)
        {
            _upButton->SetIcon(upButton);
            _downButton->SetIcon(downButton);
            _dragButton->SetIcon(dragButton);

            updatePlacement();
        }

        void ButtonPanel::SetButtonCount(uint32_t count, uint32_t buttonsPerRow)
        {
            if (count < _buttons.size())
            {
                _buttons.resize(count);
            }
            else
            {
                while (_buttons.size() < count)
                {
                    ResourcePointer<Button> button = MakeResource<Button>();
                    button->SetClickBind(_clickBind);
                    button->SetHighlightMaterial(_buttonHighlight);
                    button->SetBorder(_buttonBorder);
                    button->SetCornerBorder(_buttonCornerBorder);

                    _buttons.push_back(button);
                }
            }
            _buttonVisibility.resize(count, true);

            _rowSize = buttonsPerRow;

            updatePlacement();
        }

        void ButtonPanel::SetButtonIcon(uint32_t index, const PanelDrawable* drawable)
        {
            _buttons[index]->SetIcon(drawable);
        }

        void ButtonPanel::SetButtonBackground(uint32_t index, const PanelDrawable* background)
        {
            _buttons[index]->SetBackground(background);
        }

        void ButtonPanel::SetButtonCornerText(uint32_t index, Alignment alignment, const Graphics::Font* font, const std::string& text)
        {
            _buttons[index]->SetCornerText(alignment, font, text);
        }

        void ButtonPanel::ForceButtonHighlightOn(uint32_t index, bool force)
        {
            _buttons[index]->ForceHighlightOn(force);
        }

        void ButtonPanel::ForceButtonHighlightOff(uint32_t index, bool force)
        {
            _buttons[index]->ForceHighlightOff(force);
        }

        void ButtonPanel::SetButtonIconColor(uint32_t index, const Color& color)
        {
            _buttons[index]->SetIconColor(color);
        }

        void ButtonPanel::SetButtonVisible(uint32_t index, bool visible)
        {
            _buttons[index]->SetVisible(visible);
            _buttonVisibility[index] = visible;
        }

        void ButtonPanel::SetButtonEnabled(uint32_t index, bool enabled)
        {
            _buttons[index]->SetEnabled(enabled);
        }

        bool ButtonPanel::IsButtonHighlighted(uint32_t& outButton)
        {
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                if (_buttons[i]->IsHighlighted())
                {
                    outButton = i;
                    return true;
                }
            }

            return false;
        }

        bool ButtonPanel::IsButtonClicked(uint32_t& outButton)
        {
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                if (_buttons[i]->IsClicked())
                {
                    outButton = i;
                    return true;
                }
            }

            return false;
        }

        bool ButtonPanel::IsButtonJustClicked(uint32_t& outButton)
        {
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                if (_buttons[i]->IsJustClicked())
                {
                    outButton = i;
                    return true;
                }
            }

            return false;
        }

        bool ButtonPanel::IsButtonMouseOver(uint32_t& outButton)
        {
            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                if (_buttons[i]->IsMouseOver())
                {
                    outButton = i;
                    return true;
                }
            }

            return false;
        }

        void ButtonPanel::Update(const Input::FrameInput& input, const Camera& camera)
        {
            if (usingScrollBar())
            {
                _upButton->Update(input, camera);
                _downButton->Update(input, camera);
                _dragButton->Update(input, camera);
            }

            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->Update(input, camera);
            }

            Vector2f mousePos = input.UnProjectMousePosition(camera);
            Vector2f mouseDelta = mousePos - _prevMousePosition;

            const float _buttonScrollStep = 0.25f;
            if (_dragButton->IsClicked())
            {
                Vector2f dragTop = _upButton->GetBounds().BottomRight();
                Vector2f dragBottom = _downButton->GetBounds().TopRight();

                Vector2f dragDir = Vector2f::Normalize(dragBottom - dragTop);
                float dragLen = Vector2f::Distance(dragBottom, dragTop);

                Vector2f durDir = mousePos - dragTop;
                Vector2f prevDir = _prevMousePosition - dragTop;

                float curDragPerc = Vector2f::Dot(durDir, dragDir) / dragLen;
                float prevDragPerc = Vector2f::Dot(prevDir, dragDir) / dragLen;

                float deltaDragPerc = curDragPerc - prevDragPerc;
                _scrollPosition = Clamp(_scrollPosition + deltaDragPerc, 0.0f, 1.0f);
                updatePlacement();
            }
            else if (_upButton->IsJustClicked())
            {
                _scrollPosition = Clamp(_scrollPosition - _buttonScrollStep, 0.0f, 1.0f);
                updatePlacement();
            }
            else if (_downButton->IsJustClicked())
            {
                _scrollPosition = Clamp(_scrollPosition + _buttonScrollStep, 0.0f, 1.0f);
                updatePlacement();
            }

            if (usingScrollBar() && RotatedRectanglef::Contains(_bounds, mousePos) && _buttons.size() > 0)
            {
                const float scrollSpeed = 100.0f;
                uint32_t rows = (_buttons.size() + _rowSize - 1) / _rowSize;
                float buttonHeight = (_bounds.ExtentX * 2.0f) / _rowSize;
                float totalHeight = rows * buttonHeight;
                const float normalizedScrollSpeed = Clamp(scrollSpeed / totalHeight, 0.0f, 1.0f);
                _scrollPosition = Clamp(_scrollPosition - input.GetMouseScrollDelta().Y * normalizedScrollSpeed, 0.0f, 1.0f);
            }

            if (_dragButton->IsEnabled() && Abs(camera.GetRotation().Angle) <= Epsilon && Abs(_bounds.R.Angle) <= Epsilon)
            {
                _useScissor = true;
                _scissor.Position = camera.Project(_bounds.TopLeft());
                _scissor.Size = camera.Project(_bounds.BottomRight()) - Vector2f(_scissor.Position);
            }
            else
            {
                _useScissor = false;
            }

            _prevMousePosition = mousePos;

            updatePlacement();
        }

        void ButtonPanel::Draw(Graphics::SpriteRenderer* spriteRenderer, const Color& color) const
        {
            if (_useScissor)
            {
                spriteRenderer->SetScissorRectangle(&_scissor);
            }

            if (usingScrollBar())
            {
                _upButton->Draw(spriteRenderer, color);
                _downButton->Draw(spriteRenderer, color);
                _dragButton->Draw(spriteRenderer, color);
            }

            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                _buttons[i]->Draw(spriteRenderer, color);
            }

            if (_useScissor)
            {
                spriteRenderer->SetScissorRectangle(NULL);
            }
        }

        ButtonPanel::~ButtonPanel()
        {
        }

        bool ButtonPanel::usingScrollBar() const
        {
            Vector2f dragIconSize = _dragButton->GetIconPixelSize();
            return dragIconSize.X > 0.0f;
        }

        void ButtonPanel::updatePlacement()
        {
            RotatedRectanglef buttonBounds = _bounds;
            if (usingScrollBar())
            {
                Vector2f dragIconSize = _dragButton->GetIconPixelSize() * _scale;

                Vector2f downDir = Vector2f::Normalize(buttonBounds.BottomLeft() - buttonBounds.TopLeft());
                Vector2f rightDir = Vector2f::Normalize(buttonBounds.TopRight() - buttonBounds.TopLeft());

                buttonBounds.Center -= rightDir * (dragIconSize.X * 0.5f);
                buttonBounds.Extents.X -= dragIconSize.X * 0.5f;

                RotatedRectanglef scrollArea(buttonBounds.Center + (rightDir * (buttonBounds.Extents.X + (dragIconSize.X * 0.5f))),
                    Vector2f(dragIconSize.X * 0.5f, buttonBounds.Extents.Y), buttonBounds.R);
                RotatedRectanglef upButtonArea(scrollArea.TopLeft() + (rightDir * scrollArea.ExtentX) + (downDir * (_upButton->GetIconPixelSize().Y * 0.5f * _scale)),
                    Vector2f(scrollArea.ExtentX, _upButton->GetIconPixelSize().Y * 0.5f * _scale), buttonBounds.R);
                RotatedRectanglef downButtonArea(scrollArea.BottomLeft() + (rightDir * scrollArea.ExtentX) - (downDir * (_downButton->GetIconPixelSize().Y * 0.5f * _scale)),
                    Vector2f(scrollArea.ExtentX, _downButton->GetIconPixelSize().Y * 0.5f * _scale), buttonBounds.R);

                Vector2f dragAreaTop = upButtonArea.BottomLeft() + (rightDir * (_dragButton->GetIconPixelSize().X * 0.5f * _scale)) +
                    (downDir * (_dragButton->GetIconPixelSize().Y * 0.5f * _scale));
                Vector2f dragAreaBottom = downButtonArea.TopLeft() + (rightDir * (_dragButton->GetIconPixelSize().X * 0.5f * _scale)) -
                    (downDir * (_dragButton->GetIconPixelSize().Y * 0.5f * _scale));
                RotatedRectanglef dragArea(Vector2f::Lerp(dragAreaTop, dragAreaBottom, _scrollPosition),
                    Vector2f(scrollArea.ExtentX, _dragButton->GetIconPixelSize().Y * 0.5f), buttonBounds.R);

                _upButton->SetBounds(upButtonArea, _scale);
                _downButton->SetBounds(downButtonArea, _scale);
                _dragButton->SetBounds(dragArea, _scale);
            }

            Vector2f downDir = Vector2f::Normalize(buttonBounds.BottomLeft() - buttonBounds.TopLeft());
            Vector2f rightDir = Vector2f::Normalize(buttonBounds.TopRight() - buttonBounds.TopLeft());

            float buttonSize = (buttonBounds.ExtentX * 2.0f) / _rowSize;
            uint32_t rows = (_buttons.size() + _rowSize - 1) / _rowSize;
            float buttonsRequiredHeight = rows * buttonSize;

            float scrollableHeight = Max(buttonsRequiredHeight - (buttonBounds.ExtentY * 2.0f), 0.0);
            float scrollPos = scrollableHeight * _scrollPosition;
            Vector2f scrollTopLeft = buttonBounds.TopLeft() - (downDir * scrollPos);

            for (uint32_t i = 0; i < _buttons.size(); i++)
            {
                uint32_t row = i / _rowSize;
                uint32_t col = i % _rowSize;
                Vector2f pos = scrollTopLeft + (downDir * ((row * buttonSize) + (buttonSize * 0.5f))) +
                    (rightDir * ((col * buttonSize) + (buttonSize * 0.5f)));
                RotatedRectanglef buttonIBounds(pos, Vector2f(buttonSize * 0.5f), buttonBounds.R);
                _buttons[i]->SetBounds(buttonIBounds, _scale);
                _buttons[i]->SetVisible(_buttonVisibility[i] && RotatedRectanglef::Intersects(buttonIBounds, _bounds));
            }

            bool scrollEnabled = usingScrollBar();
            _upButton->SetEnabled(scrollEnabled);
            _downButton->SetEnabled(scrollEnabled);
            _dragButton->SetEnabled(scrollEnabled);
        }
    }
}
