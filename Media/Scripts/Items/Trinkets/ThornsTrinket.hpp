#pragma once

#include "Items/Trinkets/BasicTrinket.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string ThornsTrinketIconMatsetPath = "";

        class ThornsTrinket : public BasicTrinket
        {
        public:
            ThornsTrinket(const ItemParameters& parameters, float percentReturn)
                : BasicTrinket(parameters, "item_thorns_trinket_name", "", "")
                , _percentReturn(percentReturn)
            {
                SetIcon(ThornsTrinketIconMatsetPath, "");
                SetTooltipDescription("item_thorns_trinket_description", _percentReturn * 100.0f);
                SetTooltipFlavor("item_thorns_trinket_flavor");
            }

            virtual void OnPostRecieveDamage(Character::Character* source, const Vector2f& position, const Character::Damage& inputDamage) override
            {
                assert(GetOwner() != nullptr);

                if (source && (inputDamage.Type | Character::DamageType_Element_Physical) != 0)
                {
                    Character::Damage returnDamage(Character::DamageType_Type_Melee | Character::DamageType_Element_Physical, inputDamage.Amount * _percentReturn);
                    source->ApplyDamage(GetOwner(), position, returnDamage);
                }
            }

        private:
            float _percentReturn;
        };
    }

    template <>
    void EnumeratePreloads<Item::ThornsTrinket>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicTrinket>(preloads);
        //preloads.insert(Item::ThornsTrinketIconMatsetPath);
    }
}
