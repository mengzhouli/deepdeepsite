#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class BrewerDwarf : public Dwarf
        {
        public:
            BrewerDwarf(const CharacterParameters& parameters);
            BrewerDwarf(const CharacterParameters& parameters, const DwarfInfo& info);
        };
    }

    template <>
    void EnumeratePreloads<Character::BrewerDwarf>(PreloadSet& preloads);
}
