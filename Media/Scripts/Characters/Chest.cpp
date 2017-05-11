#include "Characters/Chest.hpp"

#include "Items/GameItems.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

#include "ContentUtility.hpp"

#include "SkeletonUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const Audio::SoundPathVector ChestItemPickupSounds = 
        {
            { "Audio/Items/UI/item_retrieving.wav", 1.0f },
        };

        template<typename T>
        bool checkItems(const Item::FindableItems& items)
        {
            for (const auto& itemClass : items)
            {
                for (const auto& item : itemClass.second)
                {
                    bool isItem = Item::GameItem<T>::IsItem(item.first);
                    assert(isItem);
                    if (!isItem)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        Chest::Chest(const CharacterParameters& parameters, const std::string& skeletonPath, const std::string& matsetPath,
                     const Item::FindableItemSet& items, OnChestOpenCallback callback)
            : BasicCharacter(parameters, skeletonPath, matsetPath)
            , _items(items)
            , _callback(callback)
            , _openSounds()
        {
            SetMoveType(MoveType_None);
            SetMoveSpeed(0.0f);

            SetDropWeaponsOnDeath(true);
            SetDropArmorsOnDeath(true);
            SetDropTrinketsOnDeath(true);
            SetDestroyItemsOnDeath(false);

            SetItemSlotCount(Item::ItemSlot::Weapon, std::numeric_limits<uint32_t>::max());
            SetItemSlotCount(Item::ItemSlot::Armor, std::numeric_limits<uint32_t>::max());
            SetItemSlotCount(Item::ItemSlot::Trinket, std::numeric_limits<uint32_t>::max());

            assert(checkItems<Item::Weapon>(_items.Weapons));
            assert(checkItems<Item::Armor>(_items.Armors));
            assert(checkItems<Item::Trinket>(_items.Trinkets));

            std::vector<Item::WeaponType> weaponTypes
            {
                Item::WeaponType_Melee_Hands,
                Item::WeaponType_Melee_1H,
                Item::WeaponType_Melee_2H,
                Item::WeaponType_Melee_Shield,
                Item::WeaponType_Ranged_Bow,
                Item::WeaponType_Ranged_Gun_1H,
                Item::WeaponType_Ranged_Gun_2H,
                Item::WeaponType_Ranged_Thrown,
            };

            std::map<Item::WeaponType, std::string> attachJoints;
            for (const auto& type : weaponTypes)
            {
                attachJoints[type] = "";
            }

            uint32_t attachIndex = 0;
            for (const auto& weaponClass : _items.Weapons)
            {
                for (const auto& weapon : weaponClass.second)
                {
                    const auto& weaponCount = weapon.second != Item::InfiniteItemCount ? weapon.second : 1;
                    for (uint32_t i = 0; i < weaponCount; i++)
                    {
                        AddWeaponAttachment(Format("attach%u", attachIndex), "origin", "weapon_attach", attachJoints);
                        attachIndex++;
                    }
                }
            }

            SetMaxHealth(30.0f);

            _pickupItemSounds.AddSounds(ChestItemPickupSounds);
        }

        const Item::FindableItemSet& Chest::GetFindableItems() const
        {
            return _items;
        }

        template<typename T>
        void Chest::equipItems(const Item::FindableItems& items)
        {
            for (const auto& itemClass : items)
            {
                for (const auto& itemInfo : itemClass.second)
                {
                    const auto& itemType = itemInfo.first;
                    const auto& itemCount = itemInfo.second != Item::InfiniteItemCount ? itemInfo.second : 1;
                    const auto& constructor = Item::GameItem<T>::GetConstructor(itemType);
                    for (uint32_t i = 0; i < itemCount; i++)
                    {
                        T* item = GiveItem(constructor);
                        item->SetInteractive(true);
                        item->SetDestroyOnInteract(true);
                        item->SetDestructionFadeTimers(0.0f, 0.5f);
                        item->SetOnInteractSound(_pickupItemSounds.GetNextSound(), { 2000.0f, 3000.0f }, 0.3f);

                        Item::LightEmitter* lightEmitter = AsA<Item::LightEmitter>(item);
                        if (lightEmitter)
                        {
                            lightEmitter->SetLightEnabled(false);
                        }
                    }
                }
            }
        }

        void Chest::equipItems()
        {
            equipItems<Item::Weapon>(_items.Weapons);
            equipItems<Item::Armor>(_items.Armors);
            equipItems<Item::Trinket>(_items.Trinkets);
        }

        void Chest::AddOpenSounds(const Audio::SoundPathVector& sounds)
        {
            _openSounds.AddSounds(sounds);
        }

        void Chest::OnSpawn()
        {
            BasicCharacter::OnSpawn();
            equipItems();
        }

        void Chest::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicCharacter::OnLoadContent(contentManager);

            _openSounds.LoadContent(contentManager);
            _pickupItemSounds.LoadContent(contentManager);
        }

        void Chest::OnUnloadContent()
        {
            BasicCharacter::OnUnloadContent();

            _openSounds.UnloadContent();
            _pickupItemSounds.UnloadContent();
        }

        void Chest::ExpelItems()
        {
            if (_callback != nullptr)
            {
                _callback(this);
            }

            for (auto lightEmitter : GetItems<Item::LightEmitter>())
            {
                lightEmitter->SetLightEnabled(true);
            }

            auto soundManager = GetLevel()->GetSoundManager();
            soundManager->PlaySinglePositionalSound(_openSounds.GetNextSound(), Audio::SoundPriority::High, GetBounds().Middle(), 2000.0f, 4500.0f, 1.0f);

            DoDeathDropItems();
        }

        static const std::string IronChestSkeletonPath = "Skeletons/Characters/Lockbox/lockbox.skel";
        static const std::string IronChestMatsetPath = "Skeletons/Characters/Lockbox/lockbox.polymatset";
        static const Audio::SoundPathVector IronChestOpenSoundPaths =
        {
            { "Audio/Characters/Lockbox/Chest_Open.ogg", 1.0f },
        };
        static const std::string IronChestGlowParticlesPath = "Particles/treasure_light_0.partsys";

        IronChest::IronChest(const CharacterParameters& parameters, const Item::FindableItemSet& items, bool startOpen, OnChestOpenCallback callback)
            : Chest(parameters, IronChestSkeletonPath, IronChestMatsetPath, items, callback)
            , _startOpen(startOpen)
            , _open(false)
            , _openAnimation()
            , _damagedAnimations()
        {
            SetEntityMask(CharacterMask_Usable);

            DisablePhysics();
            SetInvulnerable(true);

            SetSkeletonInvertedX(true);

            SetTooltip("character_lockbox_name");

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_All, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_close", 1.0f);

            _openAnimation.AddAnimation("open", 1.0f);
            _damagedAnimations.AddAnimation("hit", 1.0f);

            AddOpenSounds(IronChestOpenSoundPaths);
        }

        void IronChest::OnSpawn()
        {
            Chest::OnSpawn();

            if (_startOpen)
            {
                open(true);
            }
        }

        float IronChest::OnPreInteractedWith(Character* interactor)
        {
            if (_open)
            {
                return -1;
            }

            return 0.0f;
        }

        void IronChest::OnPostInteractedWith(Character* interactor)
        {
            open(false);
        }

        void IronChest::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            Chest::OnPostRecieveDamage(source, position, dmg);

            if (dmg.Amount > 0.0f)
            {
                PlayAnimationSet(_damagedAnimations, false, 0.1f, 0.0f);
            }
        }

        static const std::string WoodChestSkeletonPath = "Skeletons/Characters/Lockbox/lockbox_wood.skel";

        Vector2f IronChest::GetDeathImpulse(const Physics::Collision* collision) const
        {
            if (GetCollision() == collision)
            {
                return BasicCharacter::GetDeathImpulse(collision);
            }
            else
            {
                float impulse = collision->GetMass() * Random::RandomBetween(500.0f, 1500.0f);
                float angle = Random::RandomBetween(-PiOver2, 0.0f);
                Vector2f dir = Vector2f::FromRotation(angle, impulse);
                if (Animation::IsCharacterInvertedX(this))
                {
                    dir.X = -dir.X;
                }
                return dir;
            }
        }

        void IronChest::OnLoadContent(Content::ContentManager* contentManager)
        {
            Chest::OnLoadContent(contentManager);

            _glowParticles = EmplaceResource(Content::CreateParticleSystemInstance(contentManager, IronChestGlowParticlesPath));
            _glowParticles->SetScale(2.5f);
            AttachDrawable("particles_base", _glowParticles);
        }

        void IronChest::OnUnloadContent()
        {
            Chest::OnUnloadContent();

            _glowParticles.Reset();
        }

        void IronChest::OnUpdate(double totalTime, float dt)
        {
            Chest::OnUpdate(totalTime, dt);

            _glowParticles->Update(totalTime, dt);
            _glowParticles->SetLineSpawner(GetSkeleton()->GetJointPosition("particles_base"), GetSkeleton()->GetJointPosition("particles_extent"));
        }

        void IronChest::open(bool instantaneous)
        {
            if (_open)
            {
                return;
            }

            _open = true; 

            std::string anim = _openAnimation.PopNextAnimation();
            PlayAnimation(anim, false, 0.0f, instantaneous ? GetAnimationLength(anim) : 0.0f);
            ExpelItems();
            ClearTerrainAnimations(AnimationType_Idle, Pathfinding::EdgeType_All);
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_All, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_open", 1.0f);

            _glowParticles->Start();
        }

        static const std::string WoodChestMatsetPath = "Skeletons/Characters/Lockbox/lockbox_wood.polymatset";

        WoodChest::WoodChest(const CharacterParameters& parameters, const Item::FindableItemSet& items, OnChestOpenCallback callback)
            : Chest(parameters, WoodChestSkeletonPath, WoodChestMatsetPath, items, callback)
        {
            SetSkeletonJointStrength(0.0f);
            SetSkeletonInvertedX(true);

            SetTooltip("character_wood_chest_name");
            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_All, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle", 1.0f);
            _damagedAnimations.AddAnimation("hit", 1.0f);
        }

        void WoodChest::OnDeath()
        {
            ExpelItems();
            Chest::OnDeath();
        }

        void WoodChest::OnPostRecieveDamage(Character* source, const Vector2f& position, const ResolvedDamage& dmg)
        {
            Chest::OnPostRecieveDamage(source, position, dmg);

            if (dmg.Amount > 0.0f)
            {
                PlayAnimationSet(_damagedAnimations, false, 0.1f, 0.0f);
            }
        }

        Vector2f WoodChest::GetDeathImpulse(const Physics::Collision* collision) const
        {
            if (GetCollision() == collision)
            {
                return BasicCharacter::GetDeathImpulse(collision);
            }
            else
            {
                float impulse = collision->GetMass() * 1000.0f;
                float angle = Random::RandomBetween(-Pi, 0.0f);
                Vector2f dir = Vector2f::FromRotation(angle, impulse);
                return dir;
            }
        }
    }

    template <>
    void EnumeratePreloads<Character::Chest>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);
        EnumerateSoundVectorPreloads(preloads, Character::ChestItemPickupSounds);
    }

    template <>
    void EnumeratePreloads<Character::IronChest>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Chest>(preloads);

        preloads.insert(Character::IronChestSkeletonPath);
        preloads.insert(Character::IronChestMatsetPath);
        preloads.insert(Character::IronChestGlowParticlesPath);

        EnumerateSoundVectorPreloads(preloads, Character::IronChestOpenSoundPaths);
    }

    template <>
    void EnumeratePreloads<Character::WoodChest>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::Chest>(preloads);

        preloads.insert(Character::WoodChestSkeletonPath);
        preloads.insert(Character::WoodChestMatsetPath);
    }
}
