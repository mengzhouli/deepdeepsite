#pragma once

#include "TypedEnums.hpp"

namespace Dwarf
{
    BETTER_ENUM(TutorialType, uint8_t, LoadoutClickPortrait, MovingCharacters, UseAbilities, CharacterSelection, PrimarySelection);
    BETTER_ENUM(TutorialArrowCorner, uint8_t, None, TopLeft, TopRight, BottomLeft, BottomRight);
}
