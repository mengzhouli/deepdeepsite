#pragma once

#include "GameState/LevelGameState.hpp"
#include "FileSystem/FileSystem.hpp"

#include "Graphics/IDrawable.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class LevelSelectArea : public Graphics::IDrawable
        {
        public:
            LevelSelectArea(const Rectanglef& bounds, const Graphics::Font* font, const std::vector<GameState::LevelConstructor<Level::LevelInstance>>& selectableLevels,
                            Input::InputBindCode clickBind)
                : _bounds(bounds)
                , _clickBind(clickBind)
                , _texts()
                , _highlightedItem()
                , _clickedItem()
                , _font(font)
            {
                assert(_font);
                _font->AddRef();

                uint32_t textSize = (_bounds.H / selectableLevels.size()) / 2;
                Vector2f curPos = _bounds.TopLeft();

                for (uint32_t i = 0; i < selectableLevels.size(); i++)
                {
                    std::string name = FileSystem::GetFileName(selectableLevels[i].Level);
                    Graphics::PreparedText preparedText(name, _font, textSize, Color::Black, false, false);

                    levelText text;
                    text.bounds = Rectanglef(curPos, Vector2f(_bounds.W, preparedText.GetSize().Y));
                    text.text = preparedText;
                    text.constructor = selectableLevels[i];

                    _texts.push_back(text);

                    curPos.Y += text.bounds.H;
                }

                _highlightedItem = _texts.end();
                _clickedItem = _texts.end();
            }

            ~LevelSelectArea()
            {
                SafeRelease(_font);
            }

            bool IsLevelSelected() const
            {
                return _clickedItem != _texts.end();
            }

            GameState::LevelConstructor<Level::LevelInstance> GetSelectedLevel() const
            {
                assert(_clickedItem != _texts.end());
                return _clickedItem->constructor;
            }

            void Update(const Input::FrameInput& input, const Camera& camera, double totalTime, float dt)
            {
                Vector2f mousePos = camera.UnProject(input.GetMousePosition());

                _highlightedItem = _texts.end();
                _clickedItem = _texts.end();
                for (std::vector<levelText>::const_iterator i = _texts.begin(); i != _texts.end(); i++)
                {
                    if (Rectanglef::Contains(i->bounds, mousePos))
                    {
                        _highlightedItem = i;
                    }
                }

                if (input.IsBindJustPressed(_clickBind) && _highlightedItem != _texts.end())
                {
                    _clickedItem = _highlightedItem;
                }
            }

            virtual const Rectanglef& GetDrawBounds() const override
            {
                return _bounds;
            }

            virtual void Draw(Graphics::SpriteRenderer* spriteRenderer) const override
            {
                spriteRenderer->DrawSprite(_bounds, Color::Gray);

                for (std::vector<levelText>::const_iterator i = _texts.begin(); i != _texts.end(); i++)
                {
                    spriteRenderer->DrawSprite(i->bounds, (i == _highlightedItem) ? Color::White : Color::Gray);

                    spriteRenderer->DrawString(i->text, i->bounds.Position);
                }
            }

        private:
            Rectanglef _bounds;

            Input::InputBindCode _clickBind;

            struct levelText
            {
                Rectanglef bounds;
                Graphics::PreparedText text;
                GameState::LevelConstructor<Level::LevelInstance> constructor;
            };
            std::vector<levelText> _texts;

            std::vector<levelText>::const_iterator _highlightedItem;
            std::vector<levelText>::const_iterator _clickedItem;

            const Graphics::Font* _font;
        };
    }
}
