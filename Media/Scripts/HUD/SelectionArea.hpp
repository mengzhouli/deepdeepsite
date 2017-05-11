#pragma once

#include "Input/InputBinding.hpp"
#include "Input/FrameInput.hpp"
#include "Graphics/Portrait.hpp"
#include "Graphics/Text/Font.hpp"
#include "Geometry/Rectangle.hpp"
#include "Localization/StringTable.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Character/Character.hpp"
#include "Resource.hpp"
#include "Content/Preload.hpp"

#include "HUD/ButtonPanel.hpp"

#include <stdint.h>

namespace Dwarf
{
    namespace HUD
    {
        BETTER_ENUM(SelectionAreaType, uint8_t, Classic, Large, Small);

        class SelectionArea final
        {
        public:
            SelectionArea(Content::ContentManager* contentManager, const Localization::StringTable* strings, const Settings::Profile* profile,
                          Graphics::Portrait* portrait, Input::InputBindCode clickBind, SelectionAreaType type);
            ~SelectionArea();

            void SetPosition(const Vector2f& pos, float scale);
            void SetSelectedCharacter(Character::Character* character);

            void AnimateIn(bool instant);
            void AnimateOut(bool instant);

            const Rectanglef& GetBounds() const;
            bool IsMouseOver() const;
            bool IsPortraitClicked() const;
            bool IsAttackButtonJustClicked() const;
            bool IsMoveButtonJustClicked() const;
            bool IsStopButtonJustClicked() const;
            bool IsHoldPositionButtonJustClicked() const;
            bool IsAbilityButtonJustClicked(uint32_t& outAbilityIdx) const;

            Rectanglef GetAbilityButtonBounds(uint32_t abilityIdx) const;

            void Update(double totalTime, float dt, const Input::FrameInput& input, const Camera& camera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        private:
            SelectionAreaType _type;

            Input::InputBindCode _clickBind;

            Graphics::Portrait* _portrait;

            const Localization::StringTable* _stringTable;
            const Settings::Profile* _profile;

            Character::Character* _selectedCharacter;
            float _selectedCharacterSize;
            Animation::SkeletonInstance* _selectedCharacterSkeleton;

            struct actionButton
            {
                std::string icon;
                std::string nameStringCode;
                std::string tooltipStringCode;
                std::string bind;
            };
            std::vector<actionButton> _actionButtons;

            bool _mouseOver;

            bool _buttonJustClicked;
            uint32_t _clickedButton;

            bool _portraitClicked;

            const Panel* _selectedTooltip;

            const Graphics::Font* _nameFont;
            const Graphics::Font* _tooltipFont;
            Animation::SkeletonInstance* _panel;
            HUD::ButtonPanel* _buttons;
            std::vector<const Panel*> _tooltips;

            float _scale;
        };
    }

    template <>
    void EnumeratePreloads<HUD::SelectionArea>(PreloadSet& preloads);
}
