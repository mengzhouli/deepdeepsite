#include "Items/Weapons/Kitchenware.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const Audio::SoundPathVector KitchenwareCollisionSounds =
        {
            { "Audio/Characters/Debris/Debris_bouncing.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_2.wav", 1.0f },
            { "Audio/Characters/Debris/Debris_bouncing_3.wav", 1.0f },
        };

        static const std::string KitchenwareSkeletonPath = "Skeletons/Items/Weapons/Kitchen/Kitchenware.skel";
        static const std::string KitchenwareIconMatsetPath = "HUD/items.hudmatset";

        static const float KitchenwareRange = 700.0f;
        static const Rotatorf KitchenwareMinSpin = TwoPi * 100.0f;
        static const Rotatorf KitchenwareMaxSpin = (TwoPi + Pi) * 100.0f;

        static const std::vector<std::string> CutleryPaths =
        {
            "Skeletons/Items/Weapons/Kitchen/Tfork.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Tknife.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Tspoon.polymatset",
        };

        Cutlery::Cutlery(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_cutlery_name", KitchenwareSkeletonPath, CutleryPaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 2.5f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            SetIcon(KitchenwareIconMatsetPath, "icon_cutlery");

            SetTooltipDescription("item_cutlery_description");
            SetTooltipFlavor("item_cutlery_flavor");

            AddSelectionSounds(GetSmallMetalWeaponSelectionSounds());
        }

        static const std::vector<std::string> ServingToolsPaths =
        {
            "Skeletons/Items/Weapons/Kitchen/grater.polymatset",
            "Skeletons/Items/Weapons/Kitchen/knife1.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Sspoon.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Sfork.polymatset",
        };

        static const float ServingToolsRegen = 0.5f;

        ServingTools::ServingTools(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_servingtools_name", KitchenwareSkeletonPath, ServingToolsPaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 3.0f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            SetIcon(KitchenwareIconMatsetPath, "icon_servingtools");

            SetTooltipDescription("item_servingtools_description", ServingToolsRegen);
            SetTooltipFlavor("item_servingtools_flavor");

            AddSelectionSounds(GetSmallMetalWeaponSelectionSounds());
        }

        void ServingTools::OnEquip(Character::Character* newOwner)
        {
            newOwner->SetHealthRegeneration(newOwner->GetHealth().GetRegeneration() + ServingToolsRegen);
        }

        void ServingTools::OnUnEquip(Character::Character* oldOwner)
        {
            if (oldOwner)
            {
                oldOwner->SetHealthRegeneration(oldOwner->GetHealth().GetRegeneration() - ServingToolsRegen);
            }
        }

        static const std::vector<std::string> PrepToolsPaths =
        {
            "Skeletons/Items/Weapons/Kitchen/Pgrinder.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Ladle.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Wisk.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Shears.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Spat.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Slotted.polymatset",
        };

        static const float PrepToolsHealthReap = 0.5f;

        PrepTools::PrepTools(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_preptools_name", KitchenwareSkeletonPath, PrepToolsPaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 3.0f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            SetIcon(KitchenwareIconMatsetPath, "icon_preptools");

            SetTooltipDescription("item_preptools_description", PrepToolsHealthReap * 100.0f);
            SetTooltipFlavor("item_preptools_flavor");

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        void PrepTools::OnPostInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& damage)
        {
            auto owner = GetOwner();
            if (target && owner->IsCharacterAttackable(target, false))
            {
                owner->Heal(nullptr, damage.Amount * PrepToolsHealthReap);
            }
        }

        static const std::vector<std::string> ButcherToolsPaths =
        {
            "Skeletons/Items/Weapons/Kitchen/knife2.polymatset",
            "Skeletons/Items/Weapons/Kitchen/knife3.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Mtender.polymatset",
        };

        ButcherTools::ButcherTools(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_butchertools_name", KitchenwareSkeletonPath, ButcherToolsPaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 12.5f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            SetIcon(KitchenwareIconMatsetPath, "icon_butchertools");

            SetTooltipDescription("item_butchertools_description");
            SetTooltipFlavor("item_butchertools_flavor");

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::vector<std::string> CookwarePaths =
        {
            "Skeletons/Items/Weapons/Kitchen/Pot.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pan1.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pan2.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Rpin.polymatset",
        };

        Cookware::Cookware(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_cookwaretools_name", KitchenwareSkeletonPath, CookwarePaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 15.0f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            SetIcon(KitchenwareIconMatsetPath, "icon_cookware");

            SetTooltipDescription("item_cookwaretools_description");
            SetTooltipFlavor("item_cookwaretools_flavor");

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::vector<std::string> AllKitchenwarePaths =
        { 
            "Skeletons/Items/Weapons/Kitchen/grater.polymatset",
            "Skeletons/Items/Weapons/Kitchen/knife1.polymatset",
            "Skeletons/Items/Weapons/Kitchen/knife2.polymatset",
            "Skeletons/Items/Weapons/Kitchen/knife3.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Ladle.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Mtender.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pan1.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pan2.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pgrinder.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Pot.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Rpin.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Sfork.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Shears.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Slotted.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Spat.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Sspoon.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Tfork.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Tknife.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Tspoon.polymatset",
            "Skeletons/Items/Weapons/Kitchen/Wisk.polymatset",
        };

        AllKitchenware::AllKitchenware(const ItemParameters& parameters)
            : ThrowingWeapon(parameters, "item_allkitchenware_name", KitchenwareSkeletonPath, AllKitchenwarePaths)
        {
            SetAttachPoints("origin", "b");
            SetRange(KitchenwareRange);
            SetDPS(Character::Damage(Character::DamageType_Type_Projectile | Character::DamageType_Source_Crushing | Character::DamageType_Element_Physical, 1.0f));

            SetSpinLimits(KitchenwareMinSpin, KitchenwareMaxSpin);
            SetMaterialCollisionSound(KitchenwareCollisionSounds);

            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Cutlery>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::CutleryPaths.begin(), Item::CutleryPaths.end());
        preloads.insert(Item::KitchenwareIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetSmallMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::ServingTools>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::ServingToolsPaths.begin(), Item::ServingToolsPaths.end());
        preloads.insert(Item::KitchenwareIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetSmallMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::PrepTools>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::PrepToolsPaths.begin(), Item::PrepToolsPaths.end());
        preloads.insert(Item::KitchenwareIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::ButcherTools>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::ButcherToolsPaths.begin(), Item::ButcherToolsPaths.end());
        preloads.insert(Item::KitchenwareIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::Cookware>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::CookwarePaths.begin(), Item::CookwarePaths.end());
        preloads.insert(Item::KitchenwareIconMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::AllKitchenware>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::ThrowingWeapon>(preloads);
        preloads.insert(Item::KitchenwareSkeletonPath);
        preloads.insert(Item::AllKitchenwarePaths.begin(), Item::AllKitchenwarePaths.end());
        EnumerateSoundVectorPreloads(preloads, Item::KitchenwareCollisionSounds);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
