#pragma once

#include "Characters/Dwarves/Dwarf.hpp"

namespace Dwarf
{
    namespace Character
    {
        class FighterDwarf : public Dwarf
        {
        public:
            FighterDwarf(const CharacterParameters& parameters);
            FighterDwarf(const CharacterParameters& parameters, const DwarfInfo& info);

        protected:
            void OnEquipItem(Item::Item* item) override;
            void OnUnequipItem(Item::Item* item) override;

        private:
            void setupRunAnimations();
        };
    }

    template <>
    void EnumeratePreloads<Character::FighterDwarf>(PreloadSet& preloads);
}
