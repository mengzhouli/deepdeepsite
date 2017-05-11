#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class NavigatorTunic : public BasicArmor
        {
        public:
            NavigatorTunic(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::NavigatorTunic>(PreloadSet& preloads);
}
