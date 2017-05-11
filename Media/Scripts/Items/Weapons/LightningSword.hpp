#pragma once

#include "Particles/ParticleSystemInstance.hpp"

#include "Items/Weapons/BasicMeleeWeapon.hpp"

namespace Dwarf
{
    namespace Item
    {
        class LightningSword : public BasicMeleeWeapon
        {
        public:
            LightningSword(const ItemParameters& parameters);

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Particles::ParticleSystemInstance* _lightningParticles;
            Lights::LineLight _light;
        };
    }

    template <>
    void EnumeratePreloads<Item::LightningSword>(PreloadSet& preloads);
}
