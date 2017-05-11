#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BrewerTunic : public BasicArmor
        {
        public:
            BrewerTunic(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::BrewerTunic>(PreloadSet& preloads);
}
