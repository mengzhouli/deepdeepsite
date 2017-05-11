#include "Abilities/ThrowBomb.hpp"

#include "Characters/Bomb.hpp"
#include "Characters/BasicCharacter.hpp"
#include "Characters/CharacterTraits.hpp"
#include "SkeletonUtility.hpp"
#include "Physics/SkeletonCollision.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const std::string ThrowBombIconPath = "HUD/abilities.hudmatset";
        static const std::string ThrowBombCursorPath = "HUD/abilities.cursorset";

        static const std::string ThrowBombBombSkeletonPath = "Skeletons/Characters/Bomb/ironbomb.skel";
        static const std::string ThrowBombBombMatsetPath = "Skeletons/Characters/Bomb/ironbomb.polymatset";

        ThrowBomb::ThrowBomb(const AbilityParameters& parameters)
            : BasicAbility(parameters, "ability_throw_bomb_name", AbilityType_OneTarget)
            , _executing(false)

            , _validPosition(false)
            , _bombTarget(Vector2f::Zero)

            , _fuseLength(4.0f)
            , _radius(600.0f)
            , _dmg(30.0f)

            , _bombAttached(false)
            , _bomb(NULL)
        {
            SetIcon(ThrowBombIconPath, "icon_throw_bomb");
            SetCursor(ThrowBombCursorPath, "cursor_throw_bomb");
            SetCooldown(30.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_throw_bomb_tooltip", _fuseLength, _dmg);
        }

        void ThrowBomb::ClearState()
        {
            _validPosition = false;
            _drawSpline = false;
            _executing = false;

            if (_bombAttached)
            {
                Character::Character* owner = GetOwner();

                Character::Thrower* thrower = GetOwner<Character::Thrower>();
                if (thrower)
                {
                    owner->DetachDrawable(thrower->GetThrownItemAttachPointName(), _bomb);
                }

                Character::BasicCharacter* basicCharacter = GetOwner<Character::BasicCharacter>();
                if (basicCharacter)
                {
                    basicCharacter->SetWeaponAlpha(1.0f, 0.1f);
                }
            }
            _bombAttached = false;
        }

        void ThrowBomb::SetPrimaryTarget(const Vector2f& pos)
        {
            _validPosition = true; // todo
            _bombTarget = pos;
            _drawSpline = true;
        }

        bool ThrowBomb::IsInValidState() const
        {
            return _validPosition;
        }

        Character::Action ThrowBomb::GenerateAction() const
        {
            if (_validPosition)
            {
                return Character::CreateUseAbilityAction(GetID(), _bombTarget);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void ThrowBomb::ExecuteAction(const Character::Action& action)
        {
            Character::Character* owner = GetOwner();

            _bombTarget = Vector2f(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            _executing = true;

            _bombAttached = true;

            owner->SetRotation(Rotatorf(_bombTarget - owner->GetPosition()));

            Character::Thrower* thrower = AsA<Character::Thrower>(owner);
            if (thrower)
            {
                owner->AttachDrawable(thrower->GetThrownItemAttachPointName(), _bomb);
                thrower->PlayThrowAnimation();
            }

            Character::BasicCharacter* basicCharacter = GetOwner<Character::BasicCharacter>();
            if (basicCharacter)
            {
                basicCharacter->SetWeaponAlpha(0.0f, 0.1f);
            }
        }

        void ThrowBomb::CancelAction()
        {
            ClearState();
        }

        bool ThrowBomb::IsExecutingAction() const
        {
            return _executing;
        }

        void ThrowBomb::OnUpdate(double totalTime, float dt)
        {
            BasicAbility::OnUpdate(totalTime, dt);

            Character::Character* owner = GetOwner();

            if (_executing)
            {
                bool passedThrowPoint = true;
                bool animationFinished = true;
                std::string attachPoint = "";

                if (IsA<Character::Thrower>(owner))
                {
                    Character::Thrower* thrower = AsA<Character::Thrower>(owner);
                    passedThrowPoint = thrower->HasThrowReleasePassed();
                    animationFinished = thrower->HasThrowAnimationFinished();
                    attachPoint = thrower->GetThrownItemAttachPointName();
                }

                Rayf bombAttach = owner->GetAttachPoint(attachPoint);
                Animation::AttachmentInfo attachInfo =
                    Animation::GetSkeletonAttachmentInfo(Animation::IsCharacterInvertedX(owner), Animation::IsCharacterInvertedY(owner), true, bombAttach,
                        _bomb, "origin", "b");

                _bomb->SetInvertedX(attachInfo.InvertX);
                _bomb->SetInvertedY(attachInfo.InvertY);
                _bomb->SetPosition(attachInfo.Position);
                _bomb->SetRotation(attachInfo.Rotation);
                _bomb->SetScale(attachInfo.Scale);

                if (_bombAttached && (passedThrowPoint || animationFinished))
                {
                    Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                    Character::IronBomb* bomb = layer->SpawnCharacter(_bomb->GetPosition(), "bomb", NULL, Character::BindCharacterConstructor<Character::IronBomb>(owner->GetID(), _fuseLength, _radius, _dmg));

                    bomb->SetRotation(_bomb->GetRotation());
                    bomb->SetScale(_bomb->GetScale());

                    Vector2f launchPos, velocity, impulse;
                    computeTrajectory(launchPos, velocity, impulse, true);

                    bomb->ApplyLinearImpulse(impulse);

                    owner->DetachDrawable(attachPoint, _bomb);
                    _bombAttached = false;

                    StartCooldown();
                }

                _executing = !animationFinished;
                if (!_executing)
                {
                    Character::BasicCharacter* basicCharacter = GetOwner<Character::BasicCharacter>();
                    if (basicCharacter)
                    {
                        basicCharacter->SetWeaponAlpha(1.0f, 0.1f);
                    }
                }
            }

            if (_drawSpline)
            {
                if (!owner->IsMoving())
                {
                    owner->SetRotation(Rotatorf(_bombTarget - owner->GetPosition()));
                }

                const uint32_t numSteps = 32;
                const float simulationTime = 1.0f;
                const float timePerStep = simulationTime / numSteps;

                Vector2f launchPos, velocity, impulse;
                computeTrajectory(launchPos, velocity, impulse, false);

                Physics::PhysicsWorld* physicsWorld = GetOwner()->GetLevelLayer()->GetPhysicsWorld();
                float g = physicsWorld->GetGravity().Y;

                _trajectorySpline.Clear();
                for (uint32_t i = 0; i < numSteps; i++)
                {
                    float t = timePerStep * i;
                    Vector2f pos(launchPos.X + (velocity.X * t), launchPos.Y + (velocity.Y * t) + (0.5f * g * t * t));
                    _trajectorySpline.AddPoint(pos);
                }
            }
        }

        void ThrowBomb::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _bomb = Content::CreateSkeletonInstance(contentManager, ThrowBombBombSkeletonPath, ThrowBombBombMatsetPath);
        }

        void ThrowBomb::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();
            SafeRelease(_bomb);
        }

        void ThrowBomb::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);
        }

        void ThrowBomb::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);

            if (_drawSpline)
            {
                Character::Character *owner = GetOwner();
                assert(owner);

                const Camera& worldCam = owner->GetLevel()->GetCameraController().GetCamera();
                const Camera& hudCam = owner->GetLevel()->GetHUDCamera();

                uint32_t numSegments = _trajectorySpline.Length() / 5.0f;
                Vector2f prev = Camera::Transpose(worldCam, _trajectorySpline.Evalulate(0.0f), hudCam);
                for (uint32_t i = 1; i < numSegments; i++)
                {
                    float t = i / (float)numSegments;
                    Vector2f cur = Camera::Transpose(worldCam, _trajectorySpline.Evalulate(t), hudCam);

                    Color color = Color::Lerp(Color::Green, Color::Blue, t);

                    const float startFadeStart = 0.3f;
                    if (t < startFadeStart)
                    {
                        color.A = 255.0f * ((t - startFadeStart) / startFadeStart);
                    }

                    const float endFadeStart = 0.5f;
                    if (t > endFadeStart)
                    {
                        color.A = 255.0f * (1.0f - ((t - endFadeStart) / endFadeStart));
                    }

                    spriteRenderer->DrawLine(prev, cur, 4, color);

                    prev = cur;
                }
            }
        }

        static Vector2f getAdjustedAimPosition(const Vector2f& origin, const Vector2f& target, float &outRangePerc)
        {
            static const float bombRange = 3000.0f;

            float dist = Vector2f::Distance(origin, target);
            if (dist > bombRange)
            {
                Vector2f dir = Vector2f::Normalize(target - origin);
                outRangePerc = 1.0f;
                return origin + (dir * bombRange);
            }
            else
            {
                outRangePerc = dist / bombRange;
                return target;
            }
        }

        void ThrowBomb::computeTrajectory(Vector2f& outLaunchPos, Vector2f& outVelocity, Vector2f& outImpulse, bool useHandPos)
        {
            Character::Character *owner = GetOwner();
            assert(owner);

            Physics::PhysicsWorld* physicsWorld = owner->GetLevelLayer()->GetPhysicsWorld();

            float m = Physics::SkeletonCollision::ComputeMass(physicsWorld, _bomb);
            float g = physicsWorld->GetGravity().Y;

            Vector2f throwPos = owner->GetBounds().Middle();
            if (useHandPos && IsA<Character::Thrower>(owner))
            {
                throwPos = owner->GetAttachPoint(GetOwner<Character::Thrower>()->GetThrownItemAttachPointName()).Position;
            }

            float rangePerc;
            Vector2f target = getAdjustedAimPosition(throwPos, _bombTarget, rangePerc);
            Vector2f dir = target - throwPos;

            const float t = Lerp(0.1f, 1.2f, rangePerc);
            const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));
            const Vector2f impulse(vi * m);

            outLaunchPos = throwPos;
            outVelocity = vi;
            outImpulse = impulse;
        }
    }

    template <>
    void EnumeratePreloads<Ability::ThrowBomb>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        preloads.insert(Ability::ThrowBombIconPath);
        preloads.insert(Ability::ThrowBombCursorPath);
        EnumeratePreloads<Character::IronBomb>(preloads);
        preloads.insert(Ability::ThrowBombBombSkeletonPath);
        preloads.insert(Ability::ThrowBombBombMatsetPath);
    }
}
