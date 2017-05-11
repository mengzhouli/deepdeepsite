#pragma once

#include "Animation/SkeletonInstance.hpp"
#include "Physics/SkeletonCollision.hpp"
#include "Ray.hpp"

namespace Dwarf
{
    namespace Character
    {
        class Character;
    }

    namespace Physics
    {
        class PhysicsWorld;
    }

    namespace Animation
    {
        struct AttachmentInfo
        {
            bool InvertX;
            bool InvertY;
            Vector2f Position;
            float Scale;
            Rotatorf Rotation;
        };

        bool IsCharacterInvertedX(const Character::Character* character);
        bool IsCharacterInvertedY(const Character::Character* character);

        AttachmentInfo GetSkeletonAttachmentInfo(bool rootInvertX, bool rootInvertY, bool scaleResult, const Rayf& rootAttachRay,
                                                 const Animation::SkeletonInstance* attachSkeleton, const std::string& attachJointA, const std::string& attachJointB);
        AttachmentInfo GetSkeletonAttachmentInfo(const Animation::SkeletonInstance* rootSkeleton, const std::string& rootJointA, const std::string& rootJointB, bool scaleResult,
                                                 const Animation::SkeletonInstance* attachSkeleton, const std::string& attachJointA, const std::string& attachJointB);

        struct BrokenMaterial
        {
            ResourcePointer<Animation::SkeletonInstance> skeleton;
            ResourcePointer<Physics::SkeletonCollision> collision;
        };
        BrokenMaterial BreakSkeletonMaterial(Animation::SkeletonInstance* skeleton, Physics::PhysicsWorld* physicsWorld, const std::string& material);
    }
}
