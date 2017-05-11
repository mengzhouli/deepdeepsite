#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class CookDwarf : public Dwarf
        {
        public:
            CookDwarf(const CharacterParameters& parameters);
            CookDwarf(const CharacterParameters& parameters, const DwarfInfo& info);
        };
    }

    template <>
    void EnumeratePreloads<Character::CookDwarf>(PreloadSet& preloads);
}
