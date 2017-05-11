#pragma once

#include "Resource.hpp"
#include "Content/ContentManager.hpp"
#include "Camera.hpp"
#include "Character/Character.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Level/LevelInstance.hpp"

namespace Dwarf
{
    namespace HUD
    {
        class ActionDisplay : public Resource
        {
        public:
            ActionDisplay(Content::ContentManager* contentManager, const Level::LevelLayerInstance* layer);

            void SetScale(float scale);

            void OnCharactersMove(const Vector2f& position);
            void OnCharactersFollow(Character::CharacterID target);
            void OnCharactersAttack(Character::CharacterID target);
            void OnCharactersAttackMove(const Vector2f& position);
            void OnCharactersInteractCharacter(Character::CharacterID source, Character::CharacterID target);
            void OnCharactersInteractItem(Item::ItemID target);

            void Update(double totalTime, float dt, const Camera& worldCamera, const Camera& hudCamera);
            void Draw(Graphics::SpriteRenderer* spriteRenderer) const;

        protected:
            virtual ~ActionDisplay();

        private:
            struct ActionItem
            {
                Vector2f targetPosition;
                Character::CharacterID sourceCharacter;
                Character::CharacterID targetCharacter;
                Item::ItemID targetItem;
                bool interaction;

                Animation::SkeletonInstance* skeleton;
                float fadeInTimer;
                float fadeOutTimer;

                ActionItem();
            };

            const Level::LevelLayerInstance* _layer;

            const Graphics::PolygonMaterialSet* _moveMatset;
            const Graphics::PolygonMaterialSet* _attackMatset;
            const Graphics::PolygonMaterialSet* _interactMatset;
            const Animation::Skeleton* _skeleton;
            Animation::AnimationSet _animation;

            std::vector<ActionItem> _activeItems;

            float _scale;
        };
    }

    template <>
    void EnumeratePreloads<HUD::ActionDisplay>(PreloadSet& preloads);
}
