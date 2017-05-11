#pragma once

#include "Items/Trinkets/BasicTrinket.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string HealthReapTrinketIconMatsetPath = "";

        class HealthReapTrinket : public BasicTrinket
        {
        public:
            HealthReapTrinket(const ItemParameters& parameters, float percentReap)
                : BasicTrinket(parameters, "item_health_reap_trinket_name", "", "")
                , _percentReap(percentReap)
            {
                SetIcon(HealthReapTrinketIconMatsetPath, "");
                SetTooltipDescription("item_health_reap_trinket_description", _percentReap * 100.0f);
                SetTooltipFlavor("item_health_reap_trinket_flavor");
            }

            virtual void OnPostInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& damage) override
            {
                assert(GetOwner() != nullptr);

                if ((damage.Type & Character::DamageType_Element_Physical) != 0)
                {
                    GetOwner()->Heal(GetOwner(), damage.Amount * _percentReap);
                }
            }

        private:
            float _percentReap;
        };
    }

    template <>
    void EnumeratePreloads<Item::HealthReapTrinket>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicTrinket>(preloads);
        //preloads.insert(Item::HealthReapTrinketIconMatsetPath);
    }
}
