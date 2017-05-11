#pragma once

#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/HUDMaterial.hpp"
#include "Geometry/Rectangle.hpp"
#include "HUD/Icon.hpp"

#include "HUD/Button.hpp"
#include "HUD/ButtonPanel.hpp"
#include "HUD/Tooltip.hpp"
#include "CharacterSet.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class HealthPanelDrawable;

        enum class CharacterStackLocation : uint8_t
        {
            Left,
            Right,
            Center,
        };

        class CharacterSelector final
        {
        public:
            CharacterSelector(Content::ContentManager* contentManager, Input::InputBindCode clickBind, Input::InputBindCode selectAllBind, CharacterStackLocation stackLocation);
            ~CharacterSelector();

            void SetPosition(const Vector2f& left, const Vector2f& right, float scale);
            void SetCharacters(const CharacterSet& allCharacters);

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

            bool IsMouseOver() const;
            const Character::Character* GetClickedCharacter() const;
            Character::Character* GetClickedCharacter();
            const Panel* GetTooltip() const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera, const CharacterSet& selectedCharacters);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

          private:
            class CharacterPile;

            void updatePlacement();

            void addCharacterToPile(const Character::Character* character, std::shared_ptr<CharacterSelector::CharacterPile> pile);
            bool removeCharacterFromPile(const Character::Character* character);
            bool lightenPile(std::shared_ptr<CharacterSelector::CharacterPile> pile, float minimumDensity);

            typedef std::vector<std::shared_ptr<CharacterPile>> CharacterPileVector;
            typedef std::map<ResourcePointer<const Character::Character>, std::shared_ptr<CharacterPile>> CharacterPileMap;

            struct CharacterButton
            {
                ResourcePointer<Button> button;
                ResourcePointer<HealthPanelDrawable> healthDrawable;
                ResourcePointer<Character::Character> character;
                std::string tooltipText;
                ResourcePointer<Panel> tooltip;
            };

            struct CharacterStack
            {
                std::vector<CharacterButton> buttons;
                ResourcePointer<Animation::SkeletonInstance> frame;
            };

            ResourcePointer<Content::ContentManager> _contentManager;

            Vector2f _left = Vector2f(0, 0);
            Vector2f _right = Vector2f(1, 0);
            float _scale = 0.0f;

            Input::InputBindCode _clickBind = Input::InputBindCode_Invalid;
            Input::InputBindCode _selectAllBind = Input::InputBindCode_Invalid;

            CharacterStackLocation _stackLocation = CharacterStackLocation::Left;

            std::vector<CharacterStack> _stacks;

            CharacterPileVector _piles;
            CharacterPileMap _pileMap;

            ResourcePointer<const Animation::Skeleton> _frameSkeleton;
            ResourcePointer<const Graphics::PolygonMaterialSet> _frameMatset;

            ResourcePointer<const BorderPanelDrawable> _buttonBorder;
            ResourcePointer<const BorderPanelDrawable> _buttonCornerBorder;
            ResourcePointer<const PanelDrawable> _buttonHighlight;
            ResourcePointer<const Graphics::Font> _font;

            CharacterSet _allCharacters;

            Vector2u _buttonPadding = Vector2u(6, 2);
            Vector2u _buttonSize = Vector2u(74);

            float _lowHealthThreshold = 0.3f;

            bool _mouseOver = false;
            bool _justClicked = false;
            ResourcePointer<Character::Character> _clickedCharacter;
            ResourcePointer<const Panel> _currentTooltip;

            bool _animatedIn = false;

            float _animationTimer = 0.0f;
            float _animationDuration = 0.0f;
        };
    }

    template <>
    void EnumeratePreloads<HUD::CharacterSelector>(PreloadSet& preloads);
}
