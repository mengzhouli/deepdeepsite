#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BuilderTunic : public BasicArmor
        {
        public:
            BuilderTunic(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::BuilderTunic>(PreloadSet& preloads);
}
