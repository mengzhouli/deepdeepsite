#include "Characters/Arrow.hpp"

#include "Physics/CircleCollision.hpp"

#include "Item/Item.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector ArrowHitGroundSoundPaths = 
        {
            { "Audio/Items/Weapons/Arrow_Dirt_1.ogg", 1.0f },
            { "Audio/Items/Weapons/Arrow_Dirt_2.ogg", 1.0f },
            { "Audio/Items/Weapons/Arrow_Dirt_3.ogg", 1.0f },
        };
        static const float ArrowHitGroundSoundVolume = 1.0f;
        static const std::pair<float, float> ArrowHitGroundSoundRange = { 500.0f, 2000.0f };

        Arrow::Arrow(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath, const std::string& hitMatsetPath,
                     CharacterID owner, const Damage& dmg)
            : SkeletonCharacter(parameters, skeletonPath, matsetPath)
            , _owner(owner ? owner : parameters.ID)
            , _dmg(dmg)

            , _hitmatsetPath(hitMatsetPath)
            , _hitMatset(nullptr)

            , _hitSomething(false)
            , _hitCharacter(0)
            , _hitJointIdx(0)
            , _hitTransform()

            , _totalFadeTime(3.0f)
            , _totalTime(60.0f)
            , _timer(_totalTime)
            , _collisionRadius(10.0f)
            , _prevPos(Vector2f::Zero)
        {
            SetEntityMask(CharacterMask_None);
            SetMoveType(MoveType_None);

            SetSkeletonCastsShadows(false);
            EnablePhysics(true);
            SetInvulnerable(true);

            SetSkeletonRotatesWithDirection(true);
            SetSkeletonInvertsWithDirection(false);

            _hitGroundSounds.AddSounds(ArrowHitGroundSoundPaths);
        }

        Arrow::~Arrow()
        {
        }

        void Arrow::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            _hitMatset = contentManager->Load<Graphics::PolygonMaterialSet>(_hitmatsetPath);

            _hitGroundSounds.LoadContent(contentManager);
        }

        void Arrow::OnUnloadContent()
        {
            releaseHitSkeleton();

            SkeletonCharacter::OnUnloadContent();

            SafeRelease(_hitMatset);

            _hitGroundSounds.UnloadContent();
        }

        Physics::Collision* Arrow::CreateCollision()
        {
            Physics::Collision* collision = new Physics::CircleCollision(GetLevelLayer()->GetPhysicsWorld(), _collisionRadius);
            collision->SetSensor(true);
            return collision;
        }

        void Arrow::OnUpdate(double totalTime, float dt)
        {
            Vector2f curPos = GetCollision()->GetPosition();
            Vector2f velocity = curPos - _prevPos;
            _prevPos = curPos;

            Rotatorf rotation = Rotatorf(-velocity) + Rotatorf::PiOver2;

            if (!_hitSomething && velocity.LengthSquared() > Epsilon)
            {
                SetRotation(rotation);
            }

            SkeletonCharacter::OnUpdate(totalTime, dt);

            Character *owner = GetLevelLayer()->GetCharacter<Character>(_owner);

            bool hitThisFrame = _hitSomething;
            if (!hitThisFrame)
            {
                std::vector<Character*> hitTargets = GetLevelLayer()->FindIntersections<Character>(curPos);
                for (uint32_t i = 0; i < hitTargets.size(); i++)
                {
                    Character* target = hitTargets[i];
                    if (owner && owner->IsCharacterAttackable(target, target == owner->GetAttackTarget()))
                    {
                        target->ApplyDamage(owner, curPos, _dmg);
                        hitThisFrame = true;

                        if (IsA<SkeletonCharacter>(target))
                        {
                            uint32_t hitJoint = 0;
                            Animation::SkeletonInstance* hitSkeleton = AsA<SkeletonCharacter>(target)->GetSkeleton();
                            if (hitSkeleton->GetJointAt(curPos, hitJoint))
                            {
                                _hitCharacter = target->GetID();
                                _hitSkeleton = hitSkeleton;
                                _hitJointIdx = hitJoint;
                            }
                            else
                            {
                                for (auto item : target->GetItems<Item::SkeletonHaver>())
                                {
                                    uint32_t itemHitJoint = 0;
                                    Animation::SkeletonInstance* itemHitSkeleton = item->GetSkeleton();
                                    if (itemHitSkeleton != nullptr && itemHitSkeleton->GetJointAt(curPos, itemHitJoint))
                                    {
                                        _hitCharacter = target->GetID();
                                        _hitSkeleton = itemHitSkeleton;
                                        _hitJointIdx = itemHitJoint;
                                        break;
                                    }
                                }
                            }
                        }

                        if (_hitSkeleton != nullptr)
                        {
                            Animation::Transformation jointTransform = _hitSkeleton->GetJointTransformation(_hitJointIdx);

                            Vector2f hitOffset(curPos - jointTransform.Position);
                            Rotatorf jointRot = -jointTransform.Rotation;

                            if (_hitSkeleton->IsInvertedX())
                            {
                                hitOffset.X = -hitOffset.X;
                            }

                            _rotOffset = rotation;
                            if (_hitSkeleton->IsInvertedX())
                            {
                                _rotOffset.Reflect(Rotatorf::PiOver2);
                                _rotOffset += Rotatorf::Pi;
                            }

                            _hitTransform = Animation::Transformation(hitOffset, 1.0f, jointRot);

                            _hitSkeleton->AddChildDrawable(_hitJointIdx, GetSkeleton());
                            SetDrawSkeleton(false);

                            SafeAddRef(_hitSkeleton);
                        }

                        break;
                    }
                }
            }
            if (!hitThisFrame)
            {
                Vector2f hitPos, hitNormal;
                Pathfinding::EdgeType outEdgeType;
                if (GetLevelLayer()->HitTerrain(curPos, outEdgeType, Pathfinding::EdgeType_All))
                {
                    hitThisFrame = true;
                    auto soundManager = GetLevel()->GetSoundManager();
                    soundManager->PlaySinglePositionalSound(_hitGroundSounds.GetNextSound(), Audio::SoundPriority::Low, curPos, 
                                                            ArrowHitGroundSoundRange.first, ArrowHitGroundSoundRange.second, ArrowHitGroundSoundVolume);
                }
            }

            if (hitThisFrame != _hitSomething)
            {
                DisablePhysics();
                ApplyMaterialSet("shaft", _hitMatset);
                _hitSomething = hitThisFrame;
            }

            if (_hitSomething && _hitSkeleton != nullptr)
            {
                const Vector2f &skelPos = _hitSkeleton->GetPosition();

                Animation::Transformation jointTransform = _hitSkeleton->GetJointTransformation(_hitJointIdx);
                jointTransform.Scale = Vector2f::One;
                if (_hitSkeleton->IsInvertedX())
                {
                    jointTransform.Position.X = skelPos.X + (skelPos.X - jointTransform.Position.X);
                }

                Animation::Transformation arrowTransform = Animation::Transformation::Compose(jointTransform, _hitTransform);

                Vector2f position = arrowTransform.Position;
                Rotatorf rot = arrowTransform.Rotation;

                if (_hitSkeleton->IsInvertedX())
                {
                    position.X = skelPos.X + (skelPos.X - position.X);

                    rot += _rotOffset;
                    rot.Reflect(Rotatorf::PiOver2);
                    rot += Rotatorf::Pi;
                }
                else
                {
                    rot += _rotOffset;
                }

                rot.Normalize();

                SetPosition(position);
                SetRotation(rot);

                Color c = GetColor();
                c.A = _hitSkeleton->GetColor().A;
                SetColor(c);

                Character* hitCharacter = GetLevelLayer()->GetCharacter(_hitCharacter);
                if (hitCharacter == nullptr)
                {
                    releaseHitSkeleton();
                    SetColor(Color::Transparent);
                }
            }

            _timer -= dt;
            if (_timer < _totalFadeTime)
            {
                Color color = GetColor();
                color.A = Lerp(0, 255, Clamp(_timer / _totalFadeTime, 0.0f, 1.0f));
                SetColor(color);
            }

            if (_timer < 0.0f || !Polygonf::Contains(GetLevel()->GetCameraController().GetBounds(), GetPosition()))
            {
                releaseHitSkeleton();
                Terminate();
            }
        }

        void Arrow::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            GetSkeleton()->SetRotation(newRot);
        }

        void Arrow::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();
            _prevPos = GetCollision()->GetPosition();
        }

        void Arrow::OnDeath()
        {
            releaseHitSkeleton();
            SkeletonCharacter::OnDeath();
        }

        void Arrow::releaseHitSkeleton()
        {
            if (_hitSkeleton != nullptr)
            {
                _hitSkeleton->RemoveChildDrawable(_hitJointIdx, GetSkeleton());
                SafeRelease(_hitSkeleton);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::Arrow>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        EnumerateSoundVectorPreloads(preloads, Character::ArrowHitGroundSoundPaths);
    }
}
