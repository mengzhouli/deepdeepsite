#pragma once

#include "Characters/SkeletonCharacter.hpp"
#include "DamageTypes.hpp"

namespace Dwarf
{
    namespace Character
    {
        class ResourceNode : public SkeletonCharacter
        {
        public:
            ResourceNode(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& materialSetPath,
                         const Item::Resources& resources);

            virtual const App::Cursor* GetInteractCursor() const override;

            Item::Resources ExtractResources(const Item::Resources& maximumAmmount);

            const Item::Resources& GetResources() const;

        protected:
            bool CanBeInteractedWith(const Character* interactor) const override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

        private:
            const App::Cursor* _mineCursor;
            Item::Resources _resources;
        };

        class GoldNode : public ResourceNode
        {
        public:
            GoldNode(const CharacterParameters& parameters, const Item::Resources& resources);
        };
    }

    template <>
    void EnumeratePreloads<Character::ResourceNode>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::GoldNode>(PreloadSet& preloads);
}
