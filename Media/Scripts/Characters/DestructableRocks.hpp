#pragma once

#include "Characters/SkeletonCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        typedef std::function<void(void)> OnDestructionCallback;

        class DestructableRocks : public SkeletonCharacter
        {
        public:
            DestructableRocks(const CharacterParameters& parameters, const std::string& collisionName, const std::string& skeletonPath,
                              const std::string& materialSetPath, OnDestructionCallback callback);

        protected:
            void OnUpdate(double totalTime, float dt) override;

            Damage OnPreRecieveDamage(Character* source, const Vector2f& pos, const Damage& dmg) override;

        private:
            std::string _collisionName;

            float _lastDmg;
            Vector2f _lastDmgPos;

            bool _destroyed;
            float _timeDead;

            OnDestructionCallback _callback;
        };

        enum RubbleSize
        {
            RubbleSize_Small,
            RubbleSize_Medium,
            RubbleSize_Large,
        };

        class DestructableRubble : public DestructableRocks
        {
        public:
            DestructableRubble(const CharacterParameters& parameters, RubbleSize size, const std::string& collisionName, OnDestructionCallback callback);
        };

        class DestructableWall : public DestructableRocks
        {
        public:
            DestructableWall(const CharacterParameters& parameters, const std::string& collisionName, OnDestructionCallback callback);
        };
    }

    template <>
    void EnumeratePreloads<Character::DestructableRocks>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::DestructableRubble>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::DestructableWall>(PreloadSet& preloads);
}
