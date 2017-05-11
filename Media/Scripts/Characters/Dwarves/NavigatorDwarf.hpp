#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class NavigatorDwarf : public Dwarf
        {
        public:
            NavigatorDwarf(const CharacterParameters& parameters);
            NavigatorDwarf(const CharacterParameters& parameters, const DwarfInfo& info);
        };
    }

    template <>
    void EnumeratePreloads<Character::NavigatorDwarf>(PreloadSet& preloads);
}
