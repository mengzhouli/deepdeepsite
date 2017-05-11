#include "Characters/GrappleRope.hpp"

#include "Level/LevelInstance.hpp"
#include "Level/LevelLayerInstance.hpp"

#include "Physics/RopeCollision.hpp"

#include "Drawables/RopeUtility.hpp"
#include "Drawables/GrappleRopeDrawable.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector GrappleHitSoundPaths =
        {
            //{ "Audio/Abilities/Navigator/Grapple_Hook.ogg", 1.0f },
            { "Audio/Abilities/Navigator/Grapple_Hook_2.ogg", 1.0f },
        };
        static const Audio::SoundPathVector GrappleRopeSoundPaths =
        {
            { "Audio/Abilities/Navigator/Grapple_Rope.ogg", 1.0f },
        };

        GrappleRope::GrappleRope(const CharacterParameters& parameters, const Vector2f& anchor, const Vector2f& source, const Vector2f& dest, float travelTime)
            : Character(parameters)
            , _linkCount(16)
            , _curTime(0.0f)
            , _totalTime(travelTime)
            , _finished(false)

            , _createdPath(false)
            , _createPathTime(0.1f)

            , _anchor(anchor)
            , _source(source)
            , _dest(dest)

            , _curRope()
            , _targetRope()

            , _anchorHit(travelTime <= 0.0f)

            , _rope(nullptr)
            , _collision(nullptr)
            , _pathId(Pathfinding::PathItemID_Invalid)
        {
            SetEntityMask(CharacterMask_Normal | CharacterMask_Constructed);
            SetAttachToGroundOnSpawn(false);
            DisablePhysics();
            SetInvulnerable(true);
            SetAttackable(false);
            _grappleHitSounds.AddSounds(GrappleHitSoundPaths);
            _grappleRopeSounds.AddSounds(GrappleRopeSoundPaths);
        }

        GrappleRope::GrappleRope(const CharacterParameters& parameters, const Vector2f& anchor, const Vector2f& source, const Vector2f& dest)
            : GrappleRope(parameters, anchor, source, dest, 3.0f)
        {
        }

        GrappleRope::~GrappleRope()
        {
            SafeRelease(_collision);
        }

        const Vector2f& GrappleRope::GetAnchor() const
        {
            return _anchor;
        }

        const Vector2f& GrappleRope::GetSource() const
        {
            return _source;
        }

        const Vector2f& GrappleRope::GetDestination() const
        {
            return _dest;
        }

        void GrappleRope::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            levelRenderer->AddDrawable(_rope, false);
            levelRenderer->AddMinimapDrawable(_rope);
        }

        const Rectanglef& GrappleRope::GetBounds() const
        {
            if (_finished && _collision)
            {
                return _collision->GetBounds();
            }
            else
            {
                return _curRope.Bounds();
            }
        }

        void GrappleRope::OnUpdate(double totalTime, float dt)
        {
            if (!_createdPath && _curTime >= Min(_createPathTime, _totalTime))
            {
                Pathfinding::PathSystem* pathSystem = GetLevelLayer()->GetPathSystem();
                _pathId = pathSystem->AddChain(_targetRope, Pathfinding::EdgeType_Climb, Pathfinding::TerrainType::Rope);
                _createdPath = true;
            }

            if (_collision && !_finished)
            {
                _collision->SetLinkPosition(_linkCount - 1, _anchor);

                _curTime += dt;
                float settlePerc = _totalTime > 0.0f ? Clamp(_curTime / _totalTime, 0.0f, 1.0f) : 1.0f;

                _curRope = _collision->GetChain();
                for (uint32_t i = 1; i < _curRope.Size() - 1; i++)
                {
                    _curRope.UpdatePoint(Vector2f::Lerp(_curRope.At(i), _targetRope.At(i), settlePerc), i);
                }

                if (_curTime > _totalTime)
                {
                    _curRope.UpdatePoint(_dest, 0);
                    _curRope.UpdatePoint(_anchor, _curRope.Size() - 1);

                    SafeRelease(_collision);
                    _finished = true;

                    if (_currentRopeSound)
                    {
                        _currentRopeSound->Stop(0.1f);
                        _currentRopeSound = nullptr;
                    }
                }

                _rope->SetChain(_curRope);

                if (!_anchorHit)
                {
                    const Rectanglef& hookBounds = _rope->GetHookBounds();
                    if (Rectanglef::Contains(hookBounds, _dest))
                    {
                        if (_currentRopeSound)
                        {
                            _currentRopeSound->Stop(0.1f);
                            _currentRopeSound = nullptr;
                        }

                        auto soundManager = GetLevel()->GetSoundManager();
                        soundManager->PlaySinglePositionalSound(_grappleHitSounds.GetNextSound(), Audio::SoundPriority::High, _dest, 2000.0f, 4500.0f, 1.0f);
                        _anchorHit = true;
                    }
                }
            }
            else if (_collision)
            {
                _curRope = _collision->GetChain();
                _rope->SetChain(_curRope);
            }
        }

        void GrappleRope::OnLoadContent(Content::ContentManager* contentManager)
        {
            const float linkSize = (Vector2f::Distance(_anchor, _dest) / _linkCount) * 1.1f;

            Vector2f a = _source;
            Vector2f b = _source + Vector2f(linkSize, 0.0f);

            Chainf chain;
            for (uint32_t i = 0; i < _linkCount; i++)
            {
                chain.AddPoint((i % 2 == 0) ? a : b);
            }

            _rope = new Graphics::GrappleRopeDrawable(contentManager);
            _rope->SetChain(chain);

            _collision = new Physics::RopeCollision(GetLevelLayer()->GetPhysicsWorld(), chain, 25.0f);
            _collision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);
            _collision->AttachLink(0, _dest);

            _grappleHitSounds.LoadContent(contentManager);
            _grappleRopeSounds.LoadContent(contentManager);
        }

        void GrappleRope::OnUnloadContent()
        {
            SafeRelease(_rope);
            SafeRelease(_collision);

            _grappleHitSounds.UnloadContent();
            _grappleRopeSounds.UnloadContent();
        }

        void GrappleRope::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                GetLevelLayer()->GetPathSystem()->RemoveObject(_pathId);
                _pathId = 0;

                SafeRelease(_collision);
                _finished = true;

                _collision = new Physics::RopeCollision(GetLevelLayer()->GetPhysicsWorld(), _curRope, 25.0f);
                _collision->SetBehavior(Physics::CollisionBehavior_Dynamic, true);
            }
        }

        void GrappleRope::OnSpawn()
        {
            _targetRope = Graphics::ComputeRopePositions(_dest, _anchor, _linkCount, -Vector2f::UnitY, 1.25f);
            if (_totalTime > 0.0f)
            {
                auto soundManager = GetLevel()->GetSoundManager();
                _currentRopeSound = soundManager->PlayLoopingPositionalSound(_grappleRopeSounds.GetNextSound(), Audio::SoundPriority::High, _source, 2000.0f, 4500.0f, 1.0f);
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::GrappleRope>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Character>(preloads);
        EnumeratePreloads<Graphics::GrappleRopeDrawable>(preloads);

        EnumerateSoundVectorPreloads(preloads, Character::GrappleHitSoundPaths);
        EnumerateSoundVectorPreloads(preloads, Character::GrappleRopeSoundPaths);
    }
}
