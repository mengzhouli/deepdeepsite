#pragma once

#include "Particles/ParticleSystemInstance.hpp"

#include "Characters/SkeletonCharacter.hpp"

namespace Dwarf
{
    namespace Graphics
    {
        class Flame;
    }

    namespace Character
    {
        class FlameHolder : public SkeletonCharacter
        {
        public:
            FlameHolder(const CharacterParameters& parameters, const std::string& skeleton, const std::string& materialSet,
                        const std::string& lightJoint, const std::string& particleJoint, bool showParticles,
                        const Color& color, bool castsShadows, bool startOn, bool playSounds);

            float GetRadius() const;

            bool IsOn() const;
            void TurnOn();
            void TurnOff();
            void SetOn(bool on);
            void Toggle();

        protected:
            void SetLightRadius(float rad);

            void SetOnSound(const std::string& path);
            void SetOffSound(const std::string& path);
            void SetBurningSound(const std::string& path);

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            virtual void OnPostInteractedWith(Character* interactor) override;

        private:
            void forceSetOn(bool on, bool firstUpdate);

            std::shared_ptr<Graphics::Flame> _flame;
            std::string _lightJoint;
            std::string _particleJoint;
        };

        class Torch : public FlameHolder
        {
        public:
            Torch(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class WallBrazier : public FlameHolder
        {
        public:
            WallBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class FloorBrazier : public FlameHolder
        {
        public:
            FloorBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class PotBrazier : public FlameHolder
        {
        public:
            PotBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class HangingBrazier : public FlameHolder
        {
        public:
            HangingBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class SmallCandle : public FlameHolder
        {
        public:
            SmallCandle(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class LargeCandle : public FlameHolder
        {
        public:
            LargeCandle(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);
        };

        class CampFire : public FlameHolder
        {
        public:
            CampFire(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);

        protected:
            virtual void OnSpawn() override;
        };

        class CookFire : public FlameHolder
        {
        public:
            CookFire(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds);

        protected:
            virtual void OnSpawn() override;
        };
    }

    template <>
    void EnumeratePreloads<Character::FlameHolder>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::Torch>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::WallBrazier>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::FloorBrazier>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::PotBrazier>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::HangingBrazier>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::SmallCandle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::LargeCandle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::CampFire>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::CookFire>(PreloadSet& preloads);
}
