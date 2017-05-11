#include "Abilities/Grapple.hpp"

#include "Item/Weapon.hpp"
#include "Animation/SkeletonInstance.hpp"

#include "Drawables/GrappleRopeDrawable.hpp"
#include "Drawables/RopeUtility.hpp"
#include "Characters/GrappleRope.hpp"

#include "Items/Weapons/WeaponTraits.hpp"
#include "Characters/BasicCharacter.hpp"
#include "AbilityUtility.hpp"
#include "ContentUtility.hpp"

#include "AttachPoints.hpp"

namespace Dwarf
{
    namespace Ability
    {
        static const float GrappleShootVolume = 1.0f;
        static const std::pair<float, float> GrappleShootSoundRadius = std::make_pair(1500.0f, 3000.0f);
        static const Audio::SoundPathVector GrappleShootSoundPaths =
        {
            { "Audio/Items/Weapons/Navigator_GunPop.wav", 1.0f },
            { "Audio/Items/Weapons/Navigator_GunPop_2.wav", 1.0f },
            { "Audio/Items/Weapons/Navigator_GunPop_3.wav", 1.0f },
        };

        Grapple::Grapple(const AbilityParameters& parameters, bool drawPlacementRope)
            : BasicAbility(parameters, "ability_grapple_name", AbilityType_OneTarget)
            , _grappleRange(5000.0f)
            , _selectedLedgePos(Vector2f::Zero)
            , _ledgeIsValid(false)
            , _drawPlacementRopeEnabled(drawPlacementRope)
            , _draw(false)
            , _drawable(nullptr)
            , _grappleShootSounds()
        {
            SetIcon("HUD/abilities.hudmatset", "icon_grapple");
            SetCursor("HUD/abilities.cursorset", "cursor_grapple");
            SetCooldown(10.0f);
            SetResourceCost(Item::Resources(0));
            SetTooltipDescription("ability_grapple_tooltip");

            _grappleShootSounds.AddSounds(GrappleShootSoundPaths);
        }

        Grapple::Grapple(const AbilityParameters& parameters)
            : Grapple(parameters, true)
        {
        }

        void Grapple::ClearState()
        {
            _ledgeIsValid = false;
            _draw = false;

            Character::Character* owner = GetOwner();
            if (IsA<Character::BasicCharacter>(owner))
            {
                AsA<Character::BasicCharacter>(owner)->StopPointingMainArm();
            }

            for (Item::GrappleAttachable* grappleAttachableWeapon : owner->GetWeapons<Item::GrappleAttachable>())
            {
                grappleAttachableWeapon->SetGrappleAttached(false);
            }
        }

        static Vector2f getShootPos(Character::Character *owner)
        {
            std::vector<Item::MuzzleHaver*> muzzleWeapons = owner->GetWeapons<Item::MuzzleHaver>();
            if (!muzzleWeapons.empty())
            {
                return muzzleWeapons.front()->GetMuzzlePosition().Position;
            }
            else
            {
                return owner->GetAttachPoint(Character::AttachPoint_WeaponMain).Position;
            }
        }

        void Grapple::SetPrimaryTarget(const Vector2f& pos)
        {
            Vector2f groundPos;
            _ledgeIsValid = VerifyLedgePosition(pos, _selectedLedgePos, groundPos);

            if (_ledgeIsValid)
            {
                Chainf targetRope = Graphics::ComputeRopePositions(_selectedLedgePos, groundPos, 16, -Vector2f::UnitY, 1.25f);
                _drawable->SetChain(targetRope);

                _drawable->SetColor(GetAbilityUIColor(_ledgeIsValid));
                _draw = true;
            }
            else
            {
                _draw = false;
            }

            Character::Character* owner = GetOwner();
            if (IsA<Character::BasicCharacter>(owner))
            {
                AsA<Character::BasicCharacter>(owner)->PointMainArmAt(pos);
            }

            for (Item::GrappleAttachable* grappleAttachableWeapon : owner->GetWeapons<Item::GrappleAttachable>())
            {
                grappleAttachableWeapon->SetGrappleAttached(true);
            } 

            if (!owner->IsMoving())
            {
                owner->SetRotation(Rotatorf(pos - owner->GetPosition()));
            }
        }

        bool Grapple::IsInValidState() const
        {
            return _ledgeIsValid;
        }

        Character::Action Grapple::GenerateAction() const
        {
            if (_ledgeIsValid)
            {
                return Character::CreateUseAbilityAction(GetID(), _selectedLedgePos, _selectedLedgePos);
            }
            else
            {
                return Character::CreateEmptyAction();
            }
        }

        void Grapple::ExecuteAction(const Character::Action& action)
        {
            Vector2f inputPos(action.AbilityPosition.StartX, action.AbilityPosition.StartY);
            Character::Character* owner = GetOwner();

            Vector2f ledgePosition;
            Vector2f groundPosition;
            if (VerifyLedgePosition(inputPos, ledgePosition, groundPosition))
            {
                const Vector2f& shootPos = getShootPos(owner);
                Level::LevelLayerInstance* layer = owner->GetLevelLayer();
                Character::CharacterConstructor<Character::GrappleRope> constructor =
                    Character::BindCharacterConstructor<Character::GrappleRope>(groundPosition, shootPos, ledgePosition);

                layer->SpawnCharacter(groundPosition, "grapple_rope", owner->GetController(), constructor);
                auto soundManager = layer->GetLevel()->GetSoundManager();
                soundManager->PlaySinglePositionalSound(_grappleShootSounds.GetNextSound(), Audio::SoundPriority::High, shootPos, GrappleShootSoundRadius.first, GrappleShootSoundRadius.second, GrappleShootVolume);
                StartCooldown();
            }
            else
            {
                LogInfo("Invalid ledge for Grapple::ExecuteAction");
            }
        }

        void Grapple::CancelAction()
        {
        }

        bool Grapple::IsExecutingAction() const
        {
            return false;
        }

        void Grapple::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicAbility::OnLoadContent(contentManager);

            _drawable = new Graphics::GrappleRopeDrawable(contentManager);

            _grappleShootSounds.LoadContent(contentManager);
        }

        void Grapple::OnUnloadContent()
        {
            BasicAbility::OnUnloadContent();

            SafeRelease(_drawable);

            _grappleShootSounds.UnloadContent();
        }

        void Grapple::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicAbility::OnDraw(levelRenderer);

            if (_draw && _drawPlacementRopeEnabled)
            {
                levelRenderer->AddDrawable(_drawable, false, false);
            }
        }

        void Grapple::OnDrawHUD(Graphics::SpriteRenderer* spriteRenderer) const
        {
            BasicAbility::OnDrawHUD(spriteRenderer);
        }

        bool Grapple::VerifyLedgePosition(const Vector2f& inputPos, Vector2f& outLedgePos, Vector2f& outGroundPos)
        {
            bool ledgeIsValid = true;

            Character::Character* owner = GetOwner();
            Level::LevelLayerInstance* layer = owner->GetLevelLayer();

            std::shared_ptr<Pathfinding::PathPosition> ledgePos = layer->FindLedge(inputPos, 400.0f, Pathfinding::EdgeType_Walk);
            if (ledgePos != nullptr)
            {
                outLedgePos = ledgePos->GetPosition();
            }
            else
            {
                ledgeIsValid = false;
                outLedgePos = inputPos;
            }

            const Rectanglef& ownerBounds = owner->GetBounds();
            Vector2f ownerMid = ownerBounds.Middle();

            if (ownerMid.Y <= outLedgePos.Y)
            {
                ledgeIsValid = false;
            }

            float distToLedgeSq = Vector2f::DistanceSquared(ownerMid, outLedgePos);
            if (distToLedgeSq > _grappleRange * _grappleRange)
            {
                ledgeIsValid = false;
            }

            Rayf rayToLedge(ownerMid, Vector2f::Normalize(outLedgePos - ownerMid));
            std::shared_ptr<Pathfinding::PathPosition> ledgeHitPos = layer->RayCastTerrain(rayToLedge, Pathfinding::EdgeType_Walk);
            if (ledgeHitPos != nullptr && Vector2f::DistanceSquared(ownerMid, ledgeHitPos->GetPosition()) * 1.1f < distToLedgeSq)
            {
                ledgeIsValid = false;
            }

            std::shared_ptr<Pathfinding::PathPosition> groundHitPos = layer->FindPathEdge(owner->GetPosition(), 400.0f, Pathfinding::EdgeType_Walk);
            if (groundHitPos != nullptr && Vector2f::DistanceSquared(owner->GetPosition(), groundHitPos->GetPosition()) < (ownerBounds.H * ownerBounds.H * 0.1f))
            {
                outGroundPos = groundHitPos->GetPosition();
            }
            else
            {
                ledgeIsValid = false;
                outGroundPos = owner->GetPosition();
            }

            return ledgeIsValid;
        }
    }

    template <>
    void EnumeratePreloads<Ability::Grapple>(PreloadSet& preloads)
    {
        EnumeratePreloads<Ability::BasicAbility>(preloads);

        EnumeratePreloads<Graphics::GrappleRopeDrawable>(preloads);

        preloads.insert("HUD/abilities.hudmatset");
        preloads.insert("HUD/abilities.cursorset");
        EnumeratePreloads<Character::GrappleRope>(preloads);

        EnumerateSoundVectorPreloads(preloads, Ability::GrappleShootSoundPaths);
    }
}
