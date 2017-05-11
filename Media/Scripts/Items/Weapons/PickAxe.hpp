#pragma once

#include "Items/Weapons/BasicMeleeWeapon.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class PickAxe : public BasicMeleeWeapon, public Miner
        {
        public:
            PickAxe(const ItemParameters& parameters, const std::string& name, float miningRate, const std::string& matsetPath);

            // Miner interface
            virtual const Vector2f& GetTipPosition() const override;
            virtual float GetMiningRate() const override;

        private:
            float _miningRate;
        };

        class WoodPickAxe : public PickAxe
        {
        public:
            WoodPickAxe(const ItemParameters& parameters);
        };

        class IronPickAxe : public PickAxe
        {
        public:
            IronPickAxe(const ItemParameters& parameters);
        };

        class GoldPickAxe : public PickAxe
        {
        public:
            GoldPickAxe(const ItemParameters& parameters);
        };

        class DiamondPickAxe : public PickAxe
        {
        public:
            DiamondPickAxe(const ItemParameters& parameters);

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Lights::LineLight _light;

            const float _lightColorCycleTime;
            float _curLightCounterTime;
            std::vector<Color> _lightColors;
        };
    }

    template <>
    void EnumeratePreloads<Item::PickAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::WoodPickAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::IronPickAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::GoldPickAxe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::DiamondPickAxe>(PreloadSet& preloads);
}
