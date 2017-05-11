#include "Items/Weapons/PickAxe.hpp"
#include "Items/ItemSounds.hpp"
#include "Random.hpp"
#include "MathUtility.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string PickAxeSkeletonPath = "Skeletons/Items/Tools/Pickaxe/pickaxe.skel";
        static const std::string PickAxeIconMatsetPath = "HUD/items.hudmatset";

        PickAxe::PickAxe(const ItemParameters& parameters, const std::string& name, float miningRate, const std::string& matsetPath)
            : BasicMeleeWeapon(parameters, name, WeaponType_Melee_1H, PickAxeSkeletonPath, matsetPath)
            , _miningRate(miningRate)
        {
            SetAttachPoints("origin", "b");
            SetDPS(Character::Damage(Character::DamageType_Type_Melee | Character::DamageType_Source_Piercing | Character::DamageType_Element_Physical, 2.0f));
        }

        const Vector2f& PickAxe::GetTipPosition() const
        {
            return GetSkeleton()->GetJointPosition("tip_main");
        }

        float PickAxe::GetMiningRate() const
        {
            return _miningRate;
        }

        static const std::string WoodPickAxeMatsetPath = "Skeletons/Items/Tools/Pickaxe/pickaxe_wood.polymatset";

        WoodPickAxe::WoodPickAxe(const ItemParameters& parameters)
            : PickAxe(parameters, "item_woodpickaxe_name", 1.0f, WoodPickAxeMatsetPath)
        {
            SetTooltipDescription("item_woodpickaxe_description");
            SetTooltipFlavor("item_woodpickaxe_flavor");
            SetIcon(PickAxeIconMatsetPath, "icon_woodpickaxe");
            AddSelectionSounds(GetWoodWeaponSelectionSounds());
        }

        static const std::string IronPickAxeMatsetPath = "Skeletons/Items/Tools/Pickaxe/pickaxe_iron.polymatset";

        IronPickAxe::IronPickAxe(const ItemParameters& parameters)
            : PickAxe(parameters, "item_ironpickaxe_name", 1.5f, IronPickAxeMatsetPath)
        {
            SetTooltipDescription("item_ironpickaxe_description");
            SetTooltipFlavor("item_ironpickaxe_flavor");
            SetIcon(PickAxeIconMatsetPath, "icon_ironpickaxe");
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string GoldPickAxeMatsetPath = "Skeletons/Items/Tools/Pickaxe/pickaxe_gold.polymatset";

        GoldPickAxe::GoldPickAxe(const ItemParameters& parameters)
            : PickAxe(parameters, "item_goldpickaxe_name", 2.0f, GoldPickAxeMatsetPath)
        {
            SetTooltipDescription("item_goldpickaxe_description");
            SetTooltipFlavor("item_goldpickaxe_flavor");
            SetIcon(PickAxeIconMatsetPath, "icon_goldpickaxe");
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        static const std::string DiamondPickAxeMatsetPath = "Skeletons/Items/Tools/Pickaxe/pickaxe_diamond.polymatset";

        DiamondPickAxe::DiamondPickAxe(const ItemParameters& parameters)
            : PickAxe(parameters, "item_diamondpickaxe_name", 3.0f, DiamondPickAxeMatsetPath)
            , _light(Vector2f::Zero, Vector2f::Zero, 1.0f, 1.0f, Color::White, 0.0f)
            , _lightColorCycleTime(15.0f)
            , _curLightCounterTime(Random::RandomBetween(0.0f, _lightColorCycleTime))
            , _lightColors()
        {
            _lightColors.push_back(Color::FromBytes(255, 64, 64, 255));
            _lightColors.push_back(Color::FromBytes(64, 255, 64, 255));
            _lightColors.push_back(Color::FromBytes(64, 64, 255, 255));
            _lightColors.push_back(Color::FromBytes(64, 255, 255, 255));
            _lightColors.push_back(Color::FromBytes(255, 64, 255, 255));
            _lightColors.push_back(Color::FromBytes(255, 255, 64, 255));
            _lightColors.push_back(Color::FromBytes(255, 64, 64, 255));

            SetTooltipDescription("item_diamondpickaxe_description");
            SetTooltipFlavor("item_diamondpickaxe_flavor");
            SetIcon(PickAxeIconMatsetPath, "icon_diamondpickaxe");
            AddSelectionSounds(GetMetalWeaponSelectionSounds());
        }

        void DiamondPickAxe::OnUpdate(double totalTime, float dt)
        {
            BasicWeapon::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            const Vector2f& start = skeleton->GetJointPosition("tip_main");
            const Vector2f& end = skeleton->GetJointPosition("tip_off");

            _light.Start = start;
            _light.End = end;
            _light.StartRadius = 15.0f;
            _light.EndRadius = 35.0f;

            _curLightCounterTime += dt;
            float lightColorT = Mod<float>(_curLightCounterTime, _lightColorCycleTime) / _lightColorCycleTime;
            _light.LightColor = Math::EvaluateSpline(_lightColors, lightColorT);
        }

        void DiamondPickAxe::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicWeapon::OnDraw(levelRenderer);

            levelRenderer->AddLight(_light);
        }
    }

    template <>
    void EnumeratePreloads<Item::PickAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);
        preloads.insert(Item::PickAxeSkeletonPath);
    }

    template <>
    void EnumeratePreloads<Item::WoodPickAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::PickAxe>(preloads);
        preloads.insert(Item::WoodPickAxeMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetWoodWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::IronPickAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::PickAxe>(preloads);
        preloads.insert(Item::IronPickAxeMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::GoldPickAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::PickAxe>(preloads);
        preloads.insert(Item::GoldPickAxeMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::DiamondPickAxe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::PickAxe>(preloads);
        preloads.insert(Item::DiamondPickAxeMatsetPath);
        EnumerateSoundVectorPreloads(preloads, Item::GetMetalWeaponSelectionSounds());
    }
}
