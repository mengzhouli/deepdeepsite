#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class MinerTunic : public BasicArmor
        {
        public:
            MinerTunic(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::MinerTunic>(PreloadSet& preloads);
}
