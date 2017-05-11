#include "AbilityUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        Color GetAbilityUIColor(bool valid)
        {
            return valid ? Color::FromFloats(0.25f, 1.0f, 0.25f, 0.75f) : Color::FromFloats(1.0f, 0.25f, 0.25f, 0.75f);
        }
    }
}
