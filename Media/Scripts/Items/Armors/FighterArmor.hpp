#pragma once

#include "Items/Armors/BasicArmor.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Chainmail : public BasicArmor
        {
        public:
            Chainmail(const ItemParameters& parameters);
        };

        class BristleNogginArmor : public BasicArmor
        {
        public:
            BristleNogginArmor(const ItemParameters& parameters);
        };

        class BronzeArmor : public BasicArmor
        {
        public:
            BronzeArmor(const ItemParameters& parameters);
        };

        class TwinFangArmor : public BasicArmor
        {
        public:
            TwinFangArmor(const ItemParameters& parameters);
        };

        class BladeArmor : public BasicArmor
        {
        public:
            BladeArmor(const ItemParameters& parameters);
        };

        class TeapotArmor : public BasicArmor
        {
        public:
            TeapotArmor(const ItemParameters& parameters);
        };

        class WokArmor : public BasicArmor
        {
        public:
            WokArmor(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Chainmail>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BristleNogginArmor>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BronzeArmor>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::TwinFangArmor>(PreloadSet& preloads);
    
    template <>
    void EnumeratePreloads<Item::BladeArmor>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::TeapotArmor>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WokArmor>(PreloadSet& preloads);
}
