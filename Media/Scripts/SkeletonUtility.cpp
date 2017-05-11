#include "SkeletonUtility.hpp"

#include "Physics/SkeletonCollision.hpp"

#include "Characters/SkeletonCharacter.hpp"

namespace Dwarf
{
    namespace Animation
    {
        bool IsCharacterInvertedX(const Character::Character* character)
        {
            const Character::SkeletonCharacter* skeletonCharacter = AsA<Character::SkeletonCharacter>(character);
            if (skeletonCharacter != nullptr)
            {
                return skeletonCharacter->GetSkeleton()->IsInvertedX();
            }
            else
            {
                return character->GetRotation().ToVector().X > 0.0f;
            }
        }

        bool IsCharacterInvertedY(const Character::Character* character)
        {
            const Character::SkeletonCharacter* skeletonCharacter = AsA<Character::SkeletonCharacter>(character);
            if (skeletonCharacter != nullptr)
            {
                return skeletonCharacter->GetSkeleton()->IsInvertedY();
            }
            else
            {
                return false;
            }
        }

        AttachmentInfo GetSkeletonAttachmentInfo(bool rootInvertX, bool rootInvertY, bool scaleResult, const Rayf& rootAttachRay,
                                                 const Animation::SkeletonInstance* attachSkeleton, const std::string& attachJointA, const std::string& attachJointB)
        {
            AttachmentInfo result;
            result.InvertX = rootInvertX;
            result.InvertY = rootInvertY;

            const Vector2f& rootA = rootAttachRay.Position;
            Vector2f rootB = rootAttachRay.Position + rootAttachRay.Direction;

            if (attachSkeleton->HasJoint(attachJointA) && attachSkeleton->HasJoint(attachJointB))
            {
                Vector2f originalPos = const_cast<Animation::SkeletonInstance*>(attachSkeleton)->GetPosition();
                float originalScale = const_cast<Animation::SkeletonInstance*>(attachSkeleton)->GetScale();
                Rotatorf originalRot = const_cast<Animation::SkeletonInstance*>(attachSkeleton)->GetRotation();
                bool originalInvertX = const_cast<Animation::SkeletonInstance*>(attachSkeleton)->IsInvertedX();
                bool originalInvertY = const_cast<Animation::SkeletonInstance*>(attachSkeleton)->IsInvertedY();

                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetPosition(Vector2f::Zero);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetScale(1.0f);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetRotation(0.0f);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetInvertedX(rootInvertX);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetInvertedY(rootInvertY);

                const Vector2f& attachA = attachSkeleton->GetJointPosition(attachJointA);
                const Vector2f& attachB = attachSkeleton->GetJointPosition(attachJointB);

                float rootLen = Vector2f::Distance(rootA, rootB);
                float attachLen = Vector2f::Distance(attachA, attachB);
                if (attachLen > 0.0f)
                {
                    float scaleOffset = rootLen / attachLen;

                    if (scaleResult)
                    {
                        result.Scale = attachSkeleton->GetScale() * scaleOffset;
                    }
                    else
                    {
                        result.Scale = attachSkeleton->GetScale();
                    }

                    Rotatorf targetRot = Rotatorf(Vector2f::AngleBetween(attachB - attachA, rootB - rootA));
                    if (result.InvertX)
                    {
                        //targetRot += Rotatorf::Pi;
                    }
                    else
                    {
                        targetRot += Rotatorf::Pi;
                        targetRot.Reflect(Vector2f::UnitY);
                    }

                    //targetRot.Normalize();
                    result.Rotation = Rotatorf::Normalize(attachSkeleton->GetRotation() + targetRot);
                }
                else
                {
                    result.Scale = 1.0f;
                    result.Rotation = Rotatorf(rootA - rootB) + Rotatorf::PiOver2;
                }

                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetRotation(result.Rotation);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetScale(result.Scale);

                Vector2f attachJointAOffset = attachSkeleton->GetPosition() - attachSkeleton->GetJointPosition(attachJointA);
                result.Position = rootA + attachJointAOffset;

                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetPosition(originalPos);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetScale(originalScale);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetRotation(originalRot);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetInvertedX(originalInvertX);
                const_cast<Animation::SkeletonInstance*>(attachSkeleton)->SetInvertedY(originalInvertY);
            }
            else
            {
                result.Position = rootAttachRay.Position;
                result.Scale = 1.0f;
                result.Rotation = Rotatorf(rootA - rootB) + Rotatorf::PiOver2;
            }

            return result;
        }

        AttachmentInfo GetSkeletonAttachmentInfo(const Animation::SkeletonInstance* rootSkeleton, const std::string& rootJointA, const std::string& rootJointB, bool scaleResult,
                                                 const Animation::SkeletonInstance* attachSkeleton, const std::string& attachJointA, const std::string& attachJointB)
        {
            const Vector2f& rootAttachA = rootSkeleton->GetJointPosition(rootJointA);
            const Vector2f& rootAttachB = rootSkeleton->GetJointPosition(rootJointB);
            return GetSkeletonAttachmentInfo(rootSkeleton->IsInvertedX(), rootSkeleton->IsInvertedY(), scaleResult, Rayf(rootAttachA, rootAttachB - rootAttachA),
                                             attachSkeleton, attachJointA, attachJointB);
        }

        Animation::BrokenMaterial BreakSkeletonMaterial(Animation::SkeletonInstance* skeleton, Physics::PhysicsWorld* physicsWorld, const std::string& material)
        {
            assert(skeleton->HasJoint(material) && skeleton->GetMaterial(material) != nullptr);

            ResourcePointer<Animation::SkeletonInstance> instance = MakeResource<Animation::SkeletonInstance>(skeleton->GetSkeleton());
            instance->SetMaterial(material, skeleton->GetMaterial(material));
            instance->SetPosition(skeleton->GetPosition());
            instance->SetScale(skeleton->GetScale());
            instance->SetRotation(skeleton->GetRotation());
            instance->SetColor(skeleton->GetColor());
            instance->SetInvertedX(skeleton->IsInvertedX());
            instance->SetInvertedY(skeleton->IsInvertedY());
            instance->PlayAnimation(skeleton->GetCurrentAnimation(), skeleton->IsLooping(), 0.0f, skeleton->GetAnimationTime());

            ResourcePointer<Physics::SkeletonCollision> collision = MakeResource<Physics::SkeletonCollision>(physicsWorld, instance, 0.0f);
            collision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);

            skeleton->AddChildDrawable(material, instance);
            skeleton->ClearMaterial(material);

            BrokenMaterial brokenMat;
            brokenMat.skeleton = instance;
            brokenMat.collision = collision;

            return brokenMat;
        }
    }
}
