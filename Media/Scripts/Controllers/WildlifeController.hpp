#pragma once

#include "Character/Controller.hpp"
#include "Content/Preload.hpp"

namespace Dwarf
{
    namespace Character
    {
        class WildlifeController : public Controller
        {
        public:
            WildlifeController(const ControllerParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Character::WildlifeController>(PreloadSet& preloads);
}
