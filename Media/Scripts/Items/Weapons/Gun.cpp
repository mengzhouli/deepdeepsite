#include "Items/Weapons/Gun.hpp"
#include "ContentUtility.hpp"
#include "Items/ItemSounds.hpp"

#include "Random.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string GunIconMatsetPath = "HUD/items.hudmatset";
        static const std::string GunFrireParticlesPath = "Particles/gun_shot0.partsys";
        static const std::string GunSmokePath = "Particles/gun_smoke.partsys";

        Gun::Gun(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skelPath, const std::string& matsetPath, const std::string& muzzleJointName,
                 const std::string& muzzleDirJointName)
            : BasicRangedWeapon(parameters, nameCode, type, skelPath, matsetPath)

            , _explosion(nullptr)
            , _smoke(nullptr)

            , _muzzleName(muzzleJointName)
            , _muzzleDirName(muzzleDirJointName)

            , _blastLightTimer(0.0f)
            , _blastLightDuration(0.3f)
            , _explosionLight(Vector2f::Zero, 200.0f, Color::White, 0.0f)
            , _targetLight(Vector2f::Zero, Vector2f::Zero, 20.0f, 20.0f, Color::White, 0.0f)
        {
        }

        Rayf Gun::GetMuzzlePosition() const
        {
            const Vector2f& muzzleLoc = GetSkeleton()->GetJointPosition(_muzzleName);
            const Vector2f& muzzleDir = Vector2f::Normalize(GetSkeleton()->GetJointPosition(_muzzleDirName) - muzzleLoc);
            return Rayf(muzzleLoc, Vector2f::Normalize(muzzleDir));
        }

        void Gun::SetShellParticleJoint(const std::string& particlesPath, const std::string& baseJoint, const std::string& directionJoint)
        {
            _shellsPath = particlesPath;
            _shellsJoints = std::make_pair(baseJoint, directionJoint);
        }

        void Gun::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicRangedWeapon::OnLoadContent(contentManager);

            _explosion = Content::CreateParticleSystemInstance(contentManager, GunFrireParticlesPath);
            _explosion->SetScale(2.0f);

            _smoke = Content::CreateParticleSystemInstance(contentManager, GunSmokePath);
            _smoke->SetScale(2.5f);

            if (!_shellsPath.empty())
            {
                _shells = Content::CreateParticleSystemInstance(contentManager, _shellsPath);
                auto skeleton = GetSkeleton();
                skeleton->AddChildDrawable(_shellsJoints.first, _shells);
            }
        }

        void Gun::OnUnloadContent()
        {
            if (_shells)
            {
                auto skeleton = GetSkeleton();
                skeleton->RemoveChildDrawable(_shellsJoints.first, _shells);
            }

            BasicRangedWeapon::OnUnloadContent();

            SafeRelease(_explosion);
            SafeRelease(_smoke);
            SafeRelease(_shells);
        }

        void Gun::Fire(const Character::Damage& dmg)
        {
            BasicRangedWeapon::Fire(dmg);

            Character::Character *owner = GetOwner();
            assert(owner);

            Character::Character *target = GetCurrentTarget();
            assert(target);

            Rayf muzzleLoc = GetMuzzlePosition();

            const Rectanglef &targetBounds = target->GetBounds();
            Vector2f dir = targetBounds.Middle() - muzzleLoc.Position;
            float rangePerc = Clamp(dir.Length() / GetRange(), 0.0f, 1.0f);

            float maxAimVariance = 0.75f;
            float aimVariance = Lerp(0.0f, maxAimVariance, rangePerc);

            Vector2f targetAimPos = targetBounds.Middle() + Vector2f(0.0f, Random::RandomBetween(-1.0f, 1.0f) * targetBounds.H * 0.5f * aimVariance);

            Rayf shotRay(muzzleLoc.Position, Vector2f::Normalize(targetAimPos - muzzleLoc.Position));

            Vector2f targetHitPos;
            bool hitTarget = target->Intersects(shotRay, targetHitPos);

            std::shared_ptr<Pathfinding::PathPosition> hitTerrain = owner->GetLevelLayer()->RayCastTerrain(shotRay, Pathfinding::EdgeType_Walk);

            Vector2f hitPosition;
            if (hitTarget && (hitTerrain == nullptr || Vector2f::DistanceSquared(muzzleLoc.Position, targetHitPos) < Vector2f::DistanceSquared(muzzleLoc.Position, hitTerrain->GetPosition())))
            {
                // Hit the target
                hitPosition = targetHitPos;

                target->ApplyDamage(owner, hitPosition, dmg);
            }
            else if (hitTerrain != nullptr)
            {
                // Apply particles for hitting the ground
                hitPosition = hitTerrain->GetPosition();
            }

            const auto& skeleton = GetSkeleton();

            _explosion->SetPointSpawner(muzzleLoc.Position);
            _explosion->SetRotation(Rotatorf(muzzleLoc.Direction));
            _explosion->SetInvertedX(skeleton->IsInvertedX());
            _explosion->SetInvertedY(skeleton->IsInvertedY());
            _explosion->Burst();

            _smoke->SetPointSpawner(muzzleLoc.Position);
            _smoke->SetRotation(Rotatorf(muzzleLoc.Direction));
            _smoke->SetInvertedX(skeleton->IsInvertedX());
            _smoke->SetInvertedY(skeleton->IsInvertedY());
            _smoke->Burst();

            if (_shells)
            {
                const Vector2f& shellsBase = skeleton->GetJointPosition(_shellsJoints.first);
                const Vector2f& shellsDir = skeleton->GetJointPosition(_shellsJoints.second);
                Rotatorf rotation(shellsDir - shellsBase);
                _shells->SetPointSpawner(shellsBase);
                _shells->SetRotation(rotation);
                _shells->SetInvertedX(skeleton->IsInvertedX());
                _shells->SetInvertedY(skeleton->IsInvertedY());
                _shells->Burst();
            }

            _explosionLight.Position = muzzleLoc.Position;
            _targetLight.Start = muzzleLoc.Position;
            _targetLight.End = hitPosition;
            _blastLightTimer = _blastLightDuration;
        }

        void Gun::OnUpdate(double totalTime, float dt)
        {
            BasicRangedWeapon::OnUpdate(totalTime, dt);

            _explosion->Update(totalTime, dt);
            _smoke->Update(totalTime, dt);
            if (_shells)
            {
                _shells->Update(totalTime, dt);
            }

            _blastLightTimer -= dt;
            if (_blastLightTimer > 0.0f)
            {
                float fade = Clamp(_blastLightTimer / _blastLightDuration, 0.0f, 1.0f);
                Color lightBaseColor = Color(255, 204, 128, 255);
                Color lightColor = lightBaseColor * fade;
                lightColor.A = 255;

                _explosionLight.LightColor = lightColor;
                _targetLight.LightColor = lightColor;
            }
        }

        void Gun::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicRangedWeapon::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_explosion, false);
            levelRenderer->AddDrawable(_smoke, false);

            if (_blastLightTimer > 0)
            {
                levelRenderer->AddLight(_explosionLight);
                //levelRenderer->AddLight(_targetLight);
            }
        }

        static const std::string GunDefaultMuzzleJointName = "muzzle";
        static const std::string GunDefaultMuzzleDirectionJointName = "muzzle_dir";
        static const std::pair<std::string, std::string> GunDefaultAttachPoints = { "weapon_a_0", "weapon_a_1" };
        static const std::pair<std::string, std::string> GunDefaultShellAttachPoints = { "particle_base", "particle_dir" };
        static const std::string GunDefaultShellParticlesPath = "Particles/shell_0.partsys";

        static const float RifleDefaultRange = 1800.0f;

        static const std::string BasicRifleSkeletonPath = "Skeletons/Items/Weapons/Gun/rifle.skel";
        static const std::string BasicRifleMatsetPath = "Skeletons/Items/Weapons/Gun/rifle.polymatset";
        static const Audio::SoundPathVector BasicRifleShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Basic_rifle_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Basic_rifle_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Basic_rifle_3.wav", 1.0f },
        };

        BasicRifle::BasicRifle(const ItemParameters& parameters)
            : Gun(parameters, "item_rifle_name", WeaponType_Ranged_Gun_2H, BasicRifleSkeletonPath, BasicRifleMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(RifleDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 3.5f, 0.1f, 0.0f));

            SetTooltipDescription("item_rifle_description");
            SetTooltipFlavor("item_rifle_flavor");
            SetIcon(GunIconMatsetPath, "icon_rifle");

            AddFireSounds(BasicRifleShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string HandCannonRifleSkeletonPath = "Skeletons/Items/Weapons/Gun/rifle_2.skel";
        static const std::string HandCannonRifleMatsetPath = "Skeletons/Items/Weapons/Gun/rifle_2.polymatset";
        static const Audio::SoundPathVector HandCannonRifleShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Handcannon_rifle_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Handcannon_rifle_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Handcannon_rifle_3.wav", 1.0f },
        };

        HandCannonRifle::HandCannonRifle(const ItemParameters& parameters)
            : Gun(parameters, "item_hand_cannon_rifle_name", WeaponType_Ranged_Gun_2H, HandCannonRifleSkeletonPath, HandCannonRifleMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(RifleDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 5.0f));

            SetTooltipDescription("item_hand_cannon_rifle_description");
            SetTooltipFlavor("item_hand_cannon_rifle_flavor");
            SetIcon(GunIconMatsetPath, "icon_hand_cannon_rifle");

            AddFireSounds(HandCannonRifleShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string BlunderbussRifleSkeletonPath = "Skeletons/Items/Weapons/Gun/rifle_1.skel";
        static const std::string BlunderbussRifleMatsetPath = "Skeletons/Items/Weapons/Gun/rifle_1.polymatset";
        static const Audio::SoundPathVector BlunderbussRifleShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Blunderbus_rifle_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blunderbus_rifle_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blunderbus_rifle_3.wav", 1.0f },
        };

        BlunderbussRifle::BlunderbussRifle(const ItemParameters& parameters)
            : Gun(parameters, "item_blunderbuss_rifle_name", WeaponType_Ranged_Gun_2H, BlunderbussRifleSkeletonPath, BlunderbussRifleMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
            , _maxDamageReduction(0.5f)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(RifleDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 6.0f));

            SetTooltipDescription("item_blunderbuss_rifle_description", _maxDamageReduction * 100.0f);
            SetTooltipFlavor("item_blunderbuss_rifle_flavor");
            SetIcon(GunIconMatsetPath, "icon_blunderbuss_rifle");

            AddFireSounds(BlunderbussRifleShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        Character::Damage BlunderbussRifle::OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage)
        {
            float distancePerc = 1.0f - Clamp(Vector2f::Distance(position, GetPosition()) / GetRange(), 0.0f, 1.0f);
            float reduction = distancePerc * _maxDamageReduction;
            return Gun::OnPreInflictDamage(target, position, inputDamage) * (1.0f - reduction);
        }

        static const std::string BlackQueenRifleSkeletonPath = "Skeletons/Items/Weapons/Gun/rifle_3.skel";
        static const std::string BlackQueenRifleMatsetPath = "Skeletons/Items/Weapons/Gun/rifle_3.polymatset";
        static const Audio::SoundPathVector BlackQueenRifleShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Blackqueen_rifle_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blackqueen_rifle_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blackqueen_rifle_3.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blackqueen_rifle_4.wav", 1.0f },
        };

        BlackQueenRifle::BlackQueenRifle(const ItemParameters& parameters)
            : Gun(parameters, "item_black_queen_rifle_name", WeaponType_Ranged_Gun_2H, BlackQueenRifleSkeletonPath, BlackQueenRifleMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(RifleDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 4.0f, 0.0f, 3.0f));

            SetTooltipDescription("item_black_queen_rifle_description");
            SetTooltipFlavor("item_black_queen_rifle_flavor");
            SetIcon(GunIconMatsetPath, "icon_black_queen_rifle");

            AddFireSounds(BlackQueenRifleShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string PistolGrappleMatsetPath = "Skeletons/Items/Weapons/Gun/pistol_grapple.polymatset";
        static const std::string PistolFlareMatsetPath = "Skeletons/Items/Weapons/Gun/pistol_flare.polymatset";

        Pistol::Pistol(const ItemParameters& parameters, const std::string& nameCode, const std::string& skelPath, const std::string& matsetPath,
                       const std::string& muzzleJointName, const std::string& muzzleDirJointName)
            : Gun(parameters, nameCode, WeaponType_Ranged_Gun_1H, skelPath, matsetPath, muzzleJointName, muzzleDirJointName)
            , _grappleMaterial(nullptr)
            , _flareMaterial(nullptr)
        {
        }

        void Pistol::SetGrappleAttached(bool attached)
        {
            if (attached)
            {
                GetSkeleton()->MergeMaterialSet(_grappleMaterial);
            }
            else
            {
                GetSkeleton()->ClearMaterials(_grappleMaterial);
            }
        }

        void Pistol::SetFlareAttached(bool attached)
        {
            if (attached)
            {
                GetSkeleton()->MergeMaterialSet(_flareMaterial);
            }
            else
            {
                GetSkeleton()->ClearMaterials(_flareMaterial);
            }
        }

        void Pistol::OnLoadContent(Content::ContentManager* contentManager)
        {
            Gun::OnLoadContent(contentManager);

            _grappleMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(PistolGrappleMatsetPath);
            _flareMaterial = contentManager->Load<Graphics::PolygonMaterialSet>(PistolFlareMatsetPath);
        }

        void Pistol::OnUnloadContent()
        {
            Gun::OnUnloadContent();

            SafeRelease(_grappleMaterial);
            SafeRelease(_flareMaterial);
        }

        static const float PistolDefaultRange = 1300.0f;

        static const std::string BasicPistolSkeletonPath = "Skeletons/Items/Weapons/Gun/pistol.skel";
        static const std::string BasicPistolMatsetPath = "Skeletons/Items/Weapons/Gun/pistol.polymatset";
        static const Audio::SoundPathVector BasicPistolShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Basic_pistol_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Basic_pistol_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Basic_pistol_3.wav", 1.0f },
        };

        BasicPistol::BasicPistol(const ItemParameters& parameters)
            : Pistol(parameters, "item_basic_pistol_name", BasicPistolSkeletonPath, BasicPistolMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(PistolDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 2.0f));

            SetTooltipDescription("item_basic_pistol_description");
            SetTooltipFlavor("item_basic_pistol_flavor");
            SetIcon(GunIconMatsetPath, "icon_basic_pistol");

            AddFireSounds(BasicPistolShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string FingerCannonPistolSkeletonPath = "Skeletons/Items/Weapons/Gun/pistol_2.skel";
        static const std::string FingerCannonPistolMatsetPath = "Skeletons/Items/Weapons/Gun/pistol_2.polymatset";
        static const Audio::SoundPathVector FingerCannonPistolShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Fingercannon_pistol_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Fingercannon_pistol_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Fingercannon_pistol_3.wav", 1.0f },
        };

        FingerCannonPistol::FingerCannonPistol(const ItemParameters& parameters)
            : Pistol(parameters, "item_finger_cannon_pistol_name", FingerCannonPistolSkeletonPath, FingerCannonPistolMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(PistolDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 3.5f));

            SetTooltipDescription("item_finger_cannon_pistol_description");
            SetTooltipFlavor("item_finger_cannon_pistol_flavor");
            SetIcon(GunIconMatsetPath, "icon_finger_cannon_pistol");

            AddFireSounds(FingerCannonPistolShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string BlunderbussPistolSkeletonPath = "Skeletons/Items/Weapons/Gun/pistol_0.skel";
        static const std::string BlunderbussPistolMatsetPath = "Skeletons/Items/Weapons/Gun/pistol_0.polymatset";
        static const Audio::SoundPathVector BlunderbussPistolShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Blunderbus_pistol_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blunderbus_pistol_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blunderbus_pistol_3.wav", 1.0f },
        };

        BlunderbussPistol::BlunderbussPistol(const ItemParameters& parameters)
            : Pistol(parameters, "item_blunderbuss_pistol_name", BlunderbussPistolSkeletonPath, BlunderbussPistolMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
            , _maxDamageReduction(0.35f)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(PistolDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 4.5f));

            SetTooltipDescription("item_blunderbuss_pistol_description");
            SetTooltipFlavor("item_blunderbuss_pistol_flavor");
            SetIcon(GunIconMatsetPath, "icon_blunderbuss_pistol");

            AddFireSounds(BlunderbussPistolShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        Character::Damage BlunderbussPistol::OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage)
        {
            float distancePerc = 1.0f - Clamp(Vector2f::Distance(position, GetPosition()) / GetRange(), 0.0f, 1.0f);
            float reduction = distancePerc * _maxDamageReduction;
            return Gun::OnPreInflictDamage(target, position, inputDamage) * (1.0f - reduction);
        }

        static const std::string BlackPrincessPistolSkeletonPath = "Skeletons/Items/Weapons/Gun/pistol_1.skel";
        static const std::string BlackPrincessPistolMatsetPath = "Skeletons/Items/Weapons/Gun/pistol_1.polymatset";
        static const Audio::SoundPathVector BlackPrincessPistolShootSounds =
        {
            { "Audio/Items/Weapons/Wpn_Blackprincess_pistol_1.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blackprincess_pistol_2.wav", 1.0f },
            { "Audio/Items/Weapons/Wpn_Blackprincess_pistol_3.wav", 1.0f },
        };

        BlackPrincessPistol::BlackPrincessPistol(const ItemParameters& parameters)
            : Pistol(parameters, "item_black_princess_pistol_name", BlackPrincessPistolSkeletonPath, BlackPrincessPistolMatsetPath, GunDefaultMuzzleJointName, GunDefaultMuzzleDirectionJointName)
        {
            SetAttachPoints(GunDefaultAttachPoints.first, GunDefaultAttachPoints.second);
            SetRange(PistolDefaultRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 3.0f, 0.0f, 2.0f));

            SetTooltipDescription("item_black_princess_pistol_description");
            SetTooltipFlavor("item_black_princess_pistol_flavor");
            SetIcon(GunIconMatsetPath, "icon_black_princess_pistol");

            AddFireSounds(BlackPrincessPistolShootSounds);

            SetShellParticleJoint(GunDefaultShellParticlesPath, GunDefaultShellAttachPoints.first, GunDefaultShellAttachPoints.second);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Gun>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicRangedWeapon>(preloads);

        preloads.insert(Item::GunFrireParticlesPath);
        preloads.insert(Item::GunSmokePath);
    }

    template <>
    void EnumeratePreloads<Item::BasicRifle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Gun>(preloads);

        preloads.insert(Item::BasicRifleSkeletonPath);
        preloads.insert(Item::BasicRifleMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BasicRifleShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::HandCannonRifle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Gun>(preloads);

        preloads.insert(Item::HandCannonRifleSkeletonPath);
        preloads.insert(Item::HandCannonRifleMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::HandCannonRifleShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BlunderbussRifle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Gun>(preloads);

        preloads.insert(Item::BlunderbussRifleSkeletonPath);
        preloads.insert(Item::BlunderbussRifleMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BlunderbussRifleShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BlackQueenRifle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Gun>(preloads);

        preloads.insert(Item::BlackQueenRifleSkeletonPath);
        preloads.insert(Item::BlackQueenRifleMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BlackQueenRifleShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::Pistol>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Gun>(preloads);

        preloads.insert(Item::PistolGrappleMatsetPath);
        preloads.insert(Item::PistolFlareMatsetPath);

        preloads.insert(Item::GunDefaultShellParticlesPath);
    }

    template <>
    void EnumeratePreloads<Item::BasicPistol>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pistol>(preloads);

        preloads.insert(Item::BasicPistolSkeletonPath);
        preloads.insert(Item::BasicPistolMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BasicPistolShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::FingerCannonPistol>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pistol>(preloads);

        preloads.insert(Item::FingerCannonPistolSkeletonPath);
        preloads.insert(Item::FingerCannonPistolMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::FingerCannonPistolShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BlunderbussPistol>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pistol>(preloads);

        preloads.insert(Item::BlunderbussPistolSkeletonPath);
        preloads.insert(Item::BlunderbussPistolMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BlunderbussPistolShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::BlackPrincessPistol>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pistol>(preloads);

        preloads.insert(Item::BlackPrincessPistolSkeletonPath);
        preloads.insert(Item::BlackPrincessPistolMatsetPath);

        preloads.insert(Item::GunIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::BlackPrincessPistolShootSounds);

        preloads.insert(Item::GunDefaultShellParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
