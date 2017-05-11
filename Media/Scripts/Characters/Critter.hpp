#pragma once

#include "Characters/BasicCharacter.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Critter : public BasicCharacter
        {
        public:
            Critter(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath);

        protected:
            void SetFollowSpline(const Splinef& spline);

            void OnStopMoving() override;

            void OnUpdate(double totalTime, float dt) override;

        private:
            bool _followingSpline;
            Splinef _followSpline;

            float _currentTargetT;
            float _nextMoveTimer;
        };

        class Rat : public Critter
        {
        public:
            Rat(const CharacterParameters& parameters);
            Rat(const CharacterParameters& parameters, const Splinef& followSpline);
        };

        class SkeletonRat : public Critter
        {
        public:
            SkeletonRat(const CharacterParameters& parameters);
            SkeletonRat(const CharacterParameters& parameters, const Splinef& followSpline);
        };
    }

    template <>
    void EnumeratePreloads<Character::Critter>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::Rat>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Character::SkeletonRat>(PreloadSet& preloads);
}
