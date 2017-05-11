#pragma once

#include "Character/Character.hpp"
#include "Animation/SkeletonInstance.hpp"
#include "Level/LevelInstance.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Ladder : public Character
        {
        public:
            Ladder(const CharacterParameters& parameters, const Vector2f& top, const Vector2f& bottom, bool valid);

            virtual void OnSpawn() override;
            virtual void OnDeath() override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        protected:
            virtual ~Ladder();

            Vector2f _top;
            Vector2f _bottom;

            Animation::SkeletonInstance* _endA;
            std::vector<Animation::SkeletonInstance*> _segments;
            Animation::SkeletonInstance* _endB;

            Pathfinding::PathItemID _pathId;
        };
    }

    template <>
    void EnumeratePreloads<Character::Ladder>(PreloadSet& preloads);
}
