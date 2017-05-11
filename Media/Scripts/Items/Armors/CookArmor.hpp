#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Apron : public BasicArmor
        {
        public:
            Apron(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Apron>(PreloadSet& preloads);
}
