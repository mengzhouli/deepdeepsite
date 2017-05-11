#include "Items/Weapons/ThrowingWeapon.hpp"
#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        class ThrowingWeaponProjectile : public SkeletonCharacter
        {
        public:
            ThrowingWeaponProjectile(const CharacterParameters& parameters, CharacterID owner, CharacterID target, const std::string& skeletonPath, const std::string& matsetPath,
                                     const Audio::SoundPathVector& bounceSounds, const Damage& dmg)
                : SkeletonCharacter(parameters, skeletonPath, matsetPath)
                , _owner(owner ? owner : parameters.ID)
                , _target(target)
                , _dmg(dmg)
                , _hitSomething(false)
                , _totalFadeTime(1.5f)
                , _totalTime(5.0f)
                , _timer(_totalTime)
            {
                SetEntityMask(CharacterMask_None);
                SetMoveType(MoveType_None);

                SetInvulnerable(true);

                SetMaterialCollisionSound(bounceSounds);
            }

        protected:
            virtual void OnUpdate(double totalTime, float dt) override
            {
                SkeletonCharacter::OnUpdate(totalTime, dt);

                if (!_hitSomething)
                {
                    const auto& layer = GetLevelLayer();
                    Character* target = layer->GetCharacter<>(_target);

                    Vector2f hitPoint;
                    if (target != nullptr && target->Intersects(GetCollision(), hitPoint))
                    {
                        Character* owner = layer->GetCharacter<>(_owner);
                        target->ApplyDamage(owner, hitPoint, _dmg);

                        _hitSomething = true;

                        // Start fading right away
                        _timer = _totalFadeTime;
                    }
                }

                _timer -= dt;
                if (_timer < _totalFadeTime)
                {
                    Color color = GetColor();
                    color.A = Lerp(0, 255, Clamp(_timer / _totalFadeTime, 0.0f, 1.0f));
                    SetColor(color);
                }

                if (_timer < 0.0f)
                {
                    Terminate();
                }
            }

        private:
            CharacterID _owner;
            CharacterID _target;
            Damage _dmg;

            bool _hitSomething;

            const float _totalFadeTime;
            const float _totalTime;
            float _timer;
        };
    }

    template <>
    void EnumeratePreloads<Character::ThrowingWeaponProjectile>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
    }

    namespace Item
    {
        ThrowingWeapon::ThrowingWeapon(const ItemParameters& parameters, const std::string& nameCode, const std::string& skeletonPath,
                                       const std::vector<std::string>& materialPaths)
            : BasicRangedWeapon(parameters, nameCode, WeaponType_Ranged_Thrown, skeletonPath, materialPaths[0])
            , _fadeDelay(0.5f)
            , _fadeDuration(0.125f)
            , _fadeTimer(_fadeDuration)
            , _minSpin()
            , _maxSpin()
            , _skeletonPath(skeletonPath)
            , _materialPaths(materialPaths)
            , _curSkeletonIndex(0)
            , _skeletons()
        {
        }

        void ThrowingWeapon::SetSpinLimits(const Rotatorf& minRotPerSec, const Rotatorf& maxRotPerSec)
        {
            _minSpin = minRotPerSec;
            _maxSpin = maxRotPerSec;
        }

        void ThrowingWeapon::SetMaterialCollisionSound(const Audio::SoundPathVector& sounds)
        {
            BasicRangedWeapon::SetMaterialCollisionSound(sounds);
            _bounceSounds.insert(_bounceSounds.end(), sounds.begin(), sounds.end());
        }

        void ThrowingWeapon::Fire(const Character::Damage& dmg)
        {
            BasicRangedWeapon::Fire(dmg);

            Character::Character *owner = GetOwner();
            assert(owner);

            Character::Character *target = GetCurrentTarget();
            assert(target);

            Character::CharacterConstructor<Character::ThrowingWeaponProjectile> constructor =
                Character::BindCharacterConstructor<Character::ThrowingWeaponProjectile>(owner->GetID(), target->GetID(), _skeletonPath,
                    _materialPaths[_curSkeletonIndex], _bounceSounds, dmg);

            Animation::SkeletonInstance* skeleton = GetSkeleton();

            Character::ThrowingWeaponProjectile* projectile = owner->GetLevelLayer()->SpawnCharacter(skeleton->GetPosition(), "kitchenware",
                nullptr, constructor);

            // Scale an rotate the projectile to be the same as the current weapon
            projectile->SetScale(skeleton->GetScale());
            projectile->SetRotation(skeleton->GetRotation());
            projectile->EnablePhysics(true);

            Physics::Collision *projectileCollision = projectile->GetCollision();

            float m = projectileCollision->GetMass();
            float g = projectileCollision->GetPhysicsWorld()->GetGravity().Y;

            Vector2f dir = ComputeTargetAttackPoint(target) - projectile->GetPosition();

            float rangePerc = Clamp(dir.Length() / GetRange(), 0.0f, 1.0f);

            // Adjust flight time depending on range perc
            const float t = Lerp(0.05f, 0.6f, rangePerc);

            //const Vector2f vi(dir.X / t, -0.5f * g * t * t);
            const Vector2f vi(dir.X / t, ((2.0f * dir.Y) - (g * t * t)) / (2.0f * t));

            const Vector2f impulse(vi * m);

            projectile->ApplyLinearImpulse(impulse);

            float angularImpulse = m * Random::RandomBetween(_minSpin.Angle, _maxSpin.Angle);
            if (dir.X < 0.0f)
            {
                angularImpulse = -angularImpulse;
            }

            projectile->ApplyAngularImpulse(angularImpulse);

            // Set the next skeleton to throw
            _curSkeletonIndex = Random::RandomBetween(0u, _skeletons.size() - 1);
            SetSkeleton(_skeletons[_curSkeletonIndex]);

            // Make the skeleton transparent until the throw animation is complete
            _fadeTimer = -_fadeDelay;
        }

        void ThrowingWeapon::OnUpdate(double totalTime, float dt)
        {
            BasicRangedWeapon::OnUpdate(totalTime, dt);

            _fadeTimer += dt;

            Color color = GetColor();
            color.A = Clamp((_fadeTimer / _fadeDuration) * 255, 0, 255);
            SetColor(color);

            for (auto& skeleton : _skeletons)
            {
                skeleton->Update(totalTime, dt);
            }
        }

        void ThrowingWeapon::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicRangedWeapon::OnLoadContent(contentManager);

            for (const auto& materialPath : _materialPaths)
            {
                _skeletons.push_back(CreateSkeletonInstance(contentManager, _skeletonPath, materialPath));
            }
        }

        void ThrowingWeapon::OnUnloadContent()
        {
            BasicRangedWeapon::OnUnloadContent();

            SafeReleaseAndClearContainer(_skeletons);
        }
    }

    template <>
    void EnumeratePreloads<Item::ThrowingWeapon>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicRangedWeapon>(preloads);
        EnumeratePreloads<Character::ThrowingWeaponProjectile>(preloads);
    }
}
