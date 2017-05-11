#pragma once

#include "Items/Trinkets/BasicTrinket.hpp"
#include "Particles/ParticleSystemInstance.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Pipe : public BasicTrinket
        {
        public:
            Pipe(const ItemParameters& parameters, const std::string& nameCode, const std::string& matsetPath);

        protected:
            virtual void OnEquip(Character::Character* newOwner) override;
            virtual void OnUnEquip(Character::Character* oldOwner) override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

        private:
            float _nextPuffTimer;
            Particles::ParticleSystemInstance* _smoke;
        };

        class BriarPipe : public Pipe
        {
        public:
            BriarPipe(const ItemParameters& parameters);

            Character::Damage OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage) override;

        private:
            float _enemyHealthThreshold = 0.0f;
        };

        class MeerschaumPipe : public Pipe
        {
        public:
            MeerschaumPipe(const ItemParameters& parameters);

            Character::Damage OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage) override;
        };

        class SteinPipe : public Pipe
        {
        public:
            SteinPipe(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Pipe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BriarPipe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::MeerschaumPipe>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::SteinPipe>(PreloadSet& preloads);
}
