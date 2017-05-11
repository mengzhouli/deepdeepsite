#pragma once

#include "Characters/BasicCharacter.hpp"
#include "Items/GameItems.hpp"
#include "SoundSet.hpp"

#include <map>
#include <vector>
#include <string>

namespace Dwarf
{
    namespace Character
    {
        class Chest;

        typedef std::function<void(Chest*)> OnChestOpenCallback;

        class Chest : public BasicCharacter
        {
        public:
            Chest(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath,
                  const Item::FindableItemSet& items, OnChestOpenCallback callback);

            const Item::FindableItemSet& GetFindableItems() const;

        protected:
            void AddOpenSounds(const Audio::SoundPathVector& sounds);

            virtual void OnSpawn() override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            void ExpelItems();

        private:
            template<typename T> void equipItems(const Item::FindableItems& items);
            void equipItems();

            const Item::FindableItemSet _items;

            const OnChestOpenCallback _callback;

            Audio::SoundSet _openSounds;
            Audio::SoundSet _pickupItemSounds;
        };

        class IronChest : public Chest
        {
        public:
            IronChest(const CharacterParameters& parameters, const Item::FindableItemSet& items, bool startOpen, OnChestOpenCallback callback);

        protected:
            void OnSpawn() override;

            float OnPreInteractedWith(Character* interactor) override;
            void OnPostInteractedWith(Character* interactor) override;

            void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

            Vector2f GetDeathImpulse(const Physics::Collision* collision) const;

            void OnLoadContent(Content::ContentManager* contentManager) override;
            void OnUnloadContent() override;

            void OnUpdate(double totalTime, float dt) override;

        private:
            void open(bool instantaneous);

            bool _startOpen;
            bool _open;
            Animation::AnimationSet _openAnimation;
            Animation::AnimationSet _damagedAnimations;
            ResourcePointer<Particles::ParticleSystemInstance> _glowParticles;
        };

        class WoodChest : public Chest
        {
        public:
            WoodChest(const CharacterParameters& parameters, const Item::FindableItemSet& items, OnChestOpenCallback callback);

        protected:
            void OnDeath() override;

            void OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg) override;

            Vector2f GetDeathImpulse(const Physics::Collision* collision) const;

        private:
            Animation::AnimationSet _damagedAnimations;
        };
    }

    template <>
    void EnumeratePreloads<Character::Chest>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::IronChest>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::WoodChest>(PreloadSet& preloads);
}
