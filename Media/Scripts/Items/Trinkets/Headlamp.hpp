#pragma once

#include "Items/Trinkets/BasicTrinket.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Headlamp : public BasicTrinket, public LightEmitter
        {
        public:
            Headlamp(const ItemParameters& parameters, const std::string nameCode, const std::string& skeletonPath, const std::string& matsetPath);

            void SetLightEnabled(bool lightEnabled) override;
            bool IsLightEnabled() const override;

            void SetLightIntensity(float intensity);
            float GetLightIntensity() const;

        protected:
            void OnEquip(Character::Character* newOwner) override;

        private:
            float _intensity = 1.0f;
        };

        class BasicHeadlamp : public Headlamp, public MouseLooker
        {
        public:
            BasicHeadlamp(const ItemParameters& parameters);

            bool WantsToLookAtMouse() const override;

        protected:
            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Lights::PointLight _pointLight;
            Lights::SpotLight _spotLight;
        };

        class Flashlight : public Headlamp, public MouseLooker
        {
        public:
            Flashlight(const ItemParameters& parameters);

            bool WantsToLookAtMouse() const override;

        protected:
            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Lights::PointLight _pointLight;
            Lights::SpotLight _spotLight;
        };

        class Lantern : public Headlamp
        {
        public:
            Lantern(const ItemParameters& parameters);

        protected:
            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Lights::PointLight _centerLight;
            Lights::PointLight _largeLight;
        };
    }

    template <>
    void EnumeratePreloads<Item::Headlamp>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BasicHeadlamp>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::Flashlight>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::Lantern>(PreloadSet& preloads);
}
