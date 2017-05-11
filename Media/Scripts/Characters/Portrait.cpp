#include "Characters/Portrait.hpp"

#include "Abilities/GameAbilities.hpp"

#include "Characters/Dwarves/BrewerDwarf.hpp"
#include "Characters/Dwarves/BuilderDwarf.hpp"
#include "Characters/Dwarves/CookDwarf.hpp"
#include "Characters/Dwarves/FighterDwarf.hpp"
#include "Characters/Dwarves/MinerDwarf.hpp"
#include "Characters/Dwarves/NavigatorDwarf.hpp"

#include "ContentUtility.hpp"
#include "ParticlesUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string PortraitSkeletonPath = "Skeletons/UI/Portraits/Portrait.skel";
        static const std::map<std::type_index, std::pair<std::string, std::string>> PortraitMatsetPaths =
        {
            { typeid(BrewerDwarf), { "Skeletons/UI/Portraits/brewer_cp.polymatset", "brewer_type_name" } },
            { typeid(BuilderDwarf), { "Skeletons/UI/Portraits/builder_cp.polymatset", "builder_type_name" } },
            { typeid(CookDwarf), { "Skeletons/UI/Portraits/cook_cp.polymatset", "cook_type_name" } },
            { typeid(FighterDwarf), { "Skeletons/UI/Portraits/fighter_cp.polymatset", "fighter_type_name" } },
            { typeid(MinerDwarf), { "Skeletons/UI/Portraits/miner_cp.polymatset", "miner_type_name" } },
            { typeid(NavigatorDwarf), { "Skeletons/UI/Portraits/navigator_p.polymatset", "navigator_type_name" } },
        };
        static const std::string PortraitParticleJoint = "joint_0";

        static const std::string GlimmerParticlesPath = "Particles/glitter.partsys";

        Portrait::Portrait(const CharacterParameters& parameters, const std::type_index& dwarfType, OnPortraitUseCallback callback)
            : SkeletonCharacter(parameters, PortraitSkeletonPath, PortraitMatsetPaths.at(dwarfType).first)
            , _interactedWith(false)
            , _callback(callback)
            , _setParticleSpawner(false)
            , _glimmerParticles(nullptr)
        {
            SetEntityMask(CharacterMask_Usable);
            SetMoveType(MoveType_None);

            DisablePhysics();
            SetInvulnerable(true);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            SetTooltip(PortraitMatsetPaths.at(dwarfType).second);
        }

        Portrait::~Portrait()
        {
        }

        bool Portrait::HasBeenUsed() const
        {
            return _interactedWith;
        }

        void Portrait::OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos)
        {
            SkeletonCharacter::OnPositionChange(oldPos, newPos);
            updateParticleSpawner();
        }

        void Portrait::OnScaleChange(float oldScale, float newScale)
        {
            SkeletonCharacter::OnScaleChange(oldScale, newScale);
            updateParticleSpawner();
        }

        void Portrait::OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot)
        {
            SkeletonCharacter::OnRotationChange(oldRot, newRot);
            updateParticleSpawner();
        }

        void Portrait::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();

            _glimmerParticles->Start();
        }

        void Portrait::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            _glimmerParticles = Content::CreateParticleSystemInstance(contentManager, GlimmerParticlesPath);
        }

        void Portrait::OnUnloadContent()
        {
            SkeletonCharacter::OnUnloadContent();

            SafeRelease(_glimmerParticles);
        }

        float Portrait::OnPreInteractedWith(Character* interactor)
        {
            if (!_interactedWith && (!_callback || _callback(interactor)))
            {
                _interactedWith = true;
                _glimmerParticles->Stop();
                SetEntityMask(GetEntityMask() & ~CharacterMask_Usable);
                return 0.0f;
            }
            else
            {
                return -1.0f;
            }
        }

        void Portrait::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);

            if (!_setParticleSpawner)
            {
                updateParticleSpawner();
                _setParticleSpawner = true;
            }

            _glimmerParticles->Update(totalTime, dt);
        }

        void Portrait::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_glimmerParticles, false);
        }

        void Portrait::updateParticleSpawner()
        {
            Particles::FitParticlesToCharacter(_glimmerParticles, this);
        }

        AbilityPickup::AbilityPickup(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath,
            const std::string& abilityKey, bool alreadyPickedUp, OnPortraitUseCallback callback)
            : SkeletonCharacter(parameters, skeletonPath, matsetPath)
            , _abilityKey(abilityKey)
            , _ability(nullptr)
            , _bounds()
            , _interactedWith(alreadyPickedUp)
            , _callback(callback)
            , _glimmerParticles(nullptr)
        {
            SetEntityMask(CharacterMask_Usable);

            SetMoveType(MoveType_None);

            DisablePhysics();
            SetInvulnerable(true);
        }

        AbilityPickup::~AbilityPickup()
        {
        }

        void AbilityPickup::AddRemoveOnInteractMaterial(const std::string& material)
        {
            _onInteractedRemoveMaterials.push_back(material);
        }

        const HUD::PanelDrawable* AbilityPickup::GetIcon() const
        {
            return _ability->GetIcon();
        }

        const HUD::Panel* AbilityPickup::GetTooltip() const
        {
            return _ability->GetTooltip();
        }

        bool AbilityPickup::HasBeenUsed() const
        {
            return _interactedWith;
        }

        void AbilityPickup::OnSpawn()
        {
            SkeletonCharacter::OnSpawn();

            _ability = AddAbility(Ability::GameAbility::GetConstructor(_abilityKey));

            _glimmerParticles->Start();

            if (_interactedWith)
            {
                updateStateAfterInteraction();
            }
        }

        void AbilityPickup::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);

            _glimmerParticles = Content::CreateParticleSystemInstance(contentManager, GlimmerParticlesPath);
        }

        void AbilityPickup::OnUnloadContent()
        {
            SkeletonCharacter::OnUnloadContent();

            SafeRelease(_glimmerParticles);
        }

        void AbilityPickup::OnPostInteractedWith(Character* interactor)
        {
            if (!_interactedWith && (!_callback || _callback(interactor)))
            {
                _interactedWith = true;
                updateStateAfterInteraction();
            }
        }

        void AbilityPickup::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);
            Particles::FitParticlesToCharacter(_glimmerParticles, this);
            _glimmerParticles->Update(totalTime, dt);
        }

        void AbilityPickup::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_glimmerParticles, false);
        }

        void AbilityPickup::updateStateAfterInteraction()
        {
            assert(_interactedWith);
            SetEntityMask(GetEntityMask() & ~CharacterMask_Usable);
            _glimmerParticles->Stop();

            for (const auto& removeMaterial : _onInteractedRemoveMaterials)
            {
                GetSkeleton()->ClearMaterial(removeMaterial);
            }
        }

        static const std::string FlarePickupSkeletonPath = "Skeletons/Items/Doodads/flare_pickup.skel";
        static const std::string FlarePickupMatsetPath = "Skeletons/Items/Doodads/flare_pickup.polymatset";

        FlarePickup::FlarePickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback)
            : AbilityPickup(parameters, FlarePickupSkeletonPath, FlarePickupMatsetPath, Ability::Abilities::ShootFlare(), alreadyPickedUp, callback)
        {
            AddRemoveOnInteractMaterial("flares");
        }

        static const std::string GrapplePickupSkeletonPath = "Skeletons/Items/Doodads/grapple_pickup.skel";
        static const std::string GrapplePickupMatsetPath = "Skeletons/Items/Doodads/grapple_pickup.polymatset";

        GrapplePickup::GrapplePickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback)
            : AbilityPickup(parameters, GrapplePickupSkeletonPath, GrapplePickupMatsetPath, Ability::Abilities::Grapple(), alreadyPickedUp, callback)
        {
            AddRemoveOnInteractMaterial("rope");
        }

        static const std::string LeapPickupSkeletonPath = "Skeletons/Items/Doodads/leap_pickup.skel";
        static const std::string LeapPickupMatsetPath = "Skeletons/Items/Doodads/leap_pickup.polymatset";

        LeapPickup::LeapPickup(const CharacterParameters& parameters, bool alreadyPickedUp, OnPortraitUseCallback callback)
            : AbilityPickup(parameters, LeapPickupSkeletonPath, LeapPickupMatsetPath, Ability::Abilities::Leap(), alreadyPickedUp, callback)
        {
            AddRemoveOnInteractMaterial("jugs");
        }

        DrinkableType LeapPickup::GetDrinkableType() const
        {
            return DrinkableType::Beer;
        }
    }

    template <>
    void EnumeratePreloads<Character::Portrait>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        preloads.insert(Character::PortraitSkeletonPath);

        for (auto matset : Character::PortraitMatsetPaths)
        {
            preloads.insert(matset.second.first);
        }

        preloads.insert(Character::GlimmerParticlesPath);
    }

    template <>
    void EnumeratePreloads<Character::AbilityPickup>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);

        preloads.insert(Character::GlimmerParticlesPath);
    }

    template <>
    void EnumeratePreloads<Character::FlarePickup>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::AbilityPickup>(preloads);

        preloads.insert(Character::FlarePickupSkeletonPath);
        preloads.insert(Character::FlarePickupMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::GrapplePickup>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::AbilityPickup>(preloads);

        preloads.insert(Character::GrapplePickupSkeletonPath);
        preloads.insert(Character::GrapplePickupMatsetPath);
    }

    template <>
    void EnumeratePreloads<Character::LeapPickup>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::AbilityPickup>(preloads);

        preloads.insert(Character::LeapPickupSkeletonPath);
        preloads.insert(Character::LeapPickupMatsetPath);
    }
}
