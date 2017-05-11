#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "Particles/ParticleSystemInstance.hpp"

#include "Characters/CharacterTraits.hpp"

#include <typeindex>
#include <functional>

namespace Dwarf
{
    namespace Character
    {
        typedef std::function<bool(Character*)> OnPortraitUseCallback;

        class Portrait : public SkeletonCharacter
        {
        public:
            Portrait(const CharacterParameters& parameters, const std::type_index& dwarfType, OnPortraitUseCallback callback);

            bool HasBeenUsed() const;

        protected:
            virtual ~Portrait();

            virtual void OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos) override;
            virtual void OnScaleChange(float oldScale, float newScale) override;
            virtual void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override;

            virtual void OnSpawn() override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual float OnPreInteractedWith(Character* interactor) override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            void updateParticleSpawner();

            bool _interactedWith;
            OnPortraitUseCallback _callback;

            bool _setParticleSpawner;
            Particles::ParticleSystemInstance* _glimmerParticles;
        };

        class AbilityPickup : public SkeletonCharacter
        {
        public:
            AbilityPickup(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath,
                          const std::string& abilityKey, bool alreadyPickedUp, OnPortraitUseCallback callback);

            const HUD::PanelDrawable* GetIcon() const override;
            const HUD::Panel* GetTooltip() const override;

            bool HasBeenUsed() const;

        protected:
            virtual ~AbilityPickup();

            void AddRemoveOnInteractMaterial(const std::string& material);

            void OnSpawn() override;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnPostInteractedWith(Character* interactor) override;

            void OnUpdate(double totalTime, float dt) override;
            void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            void updateStateAfterInteraction();

            std::string _abilityKey;
            Ability::Ability* _ability;

            Rectanglef _bounds;

            bool _interactedWith;
            OnPortraitUseCallback _callback;

            std::vector<std::string> _onInteractedRemoveMaterials;

            Particles::ParticleSystemInstance* _glimmerParticles;
        };

        class FlarePickup : public AbilityPickup
        {
        public:
            FlarePickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback);
        };

        class GrapplePickup : public AbilityPickup
        {
        public:
            GrapplePickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback);
        };

        class LeapPickup : public AbilityPickup, public Drinkable
        {
        public:
            LeapPickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback);

            DrinkableType GetDrinkableType() const override;
        };
    }

    template <>
    void EnumeratePreloads<Character::Portrait>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::AbilityPickup>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::FlarePickup>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GrapplePickup>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::LeapPickup>(PreloadSet& preloads);
}
