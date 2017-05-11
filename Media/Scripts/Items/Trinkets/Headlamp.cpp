#include "Items/Trinkets/Headlamp.hpp"
#include "Characters/CharacterTraits.hpp"
#include "Items/ItemSounds.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string CookbookIconMatsetPath = "HUD/items.hudmatset";

        Headlamp::Headlamp(const ItemParameters& parameters, const std::string nameCode, const std::string& skeletonPath, const std::string& matsetPath)
            : BasicTrinket(parameters, nameCode, skeletonPath, matsetPath)
        {
        }

        void Headlamp::SetLightEnabled(bool lightEnabled)
        {
            _intensity = lightEnabled ? 1.0f : 0.0f;
        }

        bool Headlamp::IsLightEnabled() const
        {
            return _intensity > 0.0f;
        }

        void Headlamp::SetLightIntensity(float intensity)
        {
            _intensity = Saturate(intensity);
        }

        float Headlamp::GetLightIntensity() const
        {
            return _intensity;
        }

        void Headlamp::OnEquip(Character::Character* newOwner)
        {
            Character::LampHolder* lampHolder = AsA<Character::LampHolder>(newOwner);
            if (lampHolder != nullptr)
            {
                SetOwnerSkeletonAttachPoint(lampHolder->GetLampAttachPoint(), "origin", "b");
            }
            else
            {
                SetOwnerSkeletonAttachPoint("", "", "");
            }

            BasicTrinket::OnEquip(newOwner);
        }


        static const std::string BasicHeadlampSkeletonPath = "Skeletons/Items/Trinkets/Headlamp/basic_headlamp.skel";
        static const std::string BasicHeadlampMatsetPath = "Skeletons/Items/Trinkets/Headlamp/basic_headlamp.polymatset";

        static const Color BasicLampColor = Color::FromBytes(255, 235, 235, 255);
        static const float BasicLampRange = 3000.0f;
        static const float BasicLampPointRangePerc = 0.5f;
        static const Rotatorf BasicLampArc = Rotatorf::PiOver8 * 1.5f;
        static const float BasicLampShadowPerc = 0.7f;

        BasicHeadlamp::BasicHeadlamp(const ItemParameters& parameters)
            : Headlamp(parameters, "item_basic_headlamp_name", BasicHeadlampSkeletonPath, BasicHeadlampMatsetPath)
            , _pointLight(Vector2f::Zero, BasicLampRange * BasicLampPointRangePerc, BasicLampColor, BasicLampShadowPerc)
            , _spotLight(Vector2f::Zero, Rotatorf::Zero, BasicLampArc, 0.0f, BasicLampRange, BasicLampColor, BasicLampShadowPerc)
        {
            SetTooltipDescription("item_basic_headlamp_description");
            SetTooltipFlavor("item_basic_headlamp_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_basic_headlamp");
            AddSelectionSounds(GetMetalTrinketSelectionSounds());
        }

        bool BasicHeadlamp::WantsToLookAtMouse() const
        {
            return true;
        }

        void BasicHeadlamp::OnUpdate(double totalTime, float dt)
        {
            Headlamp::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            const Vector2f& base = skeleton->GetJointPosition("origin");
            const Vector2f& direction = skeleton->GetJointPosition("b");

            float characterAlpha = GetColor().A / 255.0f;

            _pointLight.Position = base;
            _pointLight.LightColor = BasicLampColor * characterAlpha * GetLightIntensity();

            _spotLight.Position = base;
            _spotLight.StartRadius = Vector2f::Distance(base, direction);
            _spotLight.Direction = Rotatorf(direction - base);
            _spotLight.LightColor = BasicLampColor * characterAlpha * GetLightIntensity();
        }

        void BasicHeadlamp::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Headlamp::OnDraw(levelRenderer);

            if (IsLightEnabled())
            {
                levelRenderer->AddLight(_pointLight);
                levelRenderer->AddLight(_spotLight);
            }
        }

        static const std::string FlashlightSkeletonPath = "Skeletons/Items/Trinkets/Headlamp/flashlight.skel";
        static const std::string FlashlightMatsetPath = "Skeletons/Items/Trinkets/Headlamp/flashlight.polymatset";

        static const Color FlashlightColor = Color::FromBytes(255, 245, 245, 255);
        static const float FlashlightRange = 4000.0f;
        static const float FlashlightPointRangePerc = 0.1f;
        static const Rotatorf FlashlightArc = Rotatorf::PiOver8 * 0.75f;
        static const float FlashlightShadowPerc = 0.85f;

        Flashlight::Flashlight(const ItemParameters& parameters)
            : Headlamp(parameters, "item_flashlight_name", FlashlightSkeletonPath, FlashlightMatsetPath)
            , _pointLight(Vector2f::Zero, FlashlightRange * FlashlightPointRangePerc, FlashlightColor, FlashlightShadowPerc)
            , _spotLight(Vector2f::Zero, Rotatorf::Zero, FlashlightArc, 0.0f, FlashlightRange, FlashlightColor, FlashlightShadowPerc)
        {
            SetTooltipDescription("item_flashlight_description");
            SetTooltipFlavor("item_flashlight_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_flashlight");
            AddSelectionSounds(GetMetalTrinketSelectionSounds());
        }

        bool Flashlight::WantsToLookAtMouse() const
        {
            return true;
        }

        void Flashlight::OnUpdate(double totalTime, float dt)
        {
            Headlamp::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            const Vector2f& base = skeleton->GetJointPosition("origin");
            const Vector2f& direction = skeleton->GetJointPosition("light_end");

            float characterAlpha = GetColor().A / 255.0f;

            _pointLight.Position = direction;
            _pointLight.LightColor = FlashlightColor * characterAlpha * GetLightIntensity();

            _spotLight.Position = base;
            _spotLight.StartRadius = Vector2f::Distance(base, direction);
            _spotLight.Direction = Rotatorf(direction - base);
            _spotLight.LightColor = FlashlightColor * characterAlpha * GetLightIntensity();
        }

        void Flashlight::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Headlamp::OnDraw(levelRenderer);

            if (IsLightEnabled())
            {
                levelRenderer->AddLight(_pointLight);
                levelRenderer->AddLight(_spotLight);
            }
        }

        static const std::string LanternSkeletonPath = "Skeletons/Items/Trinkets/Headlamp/lantern.skel";
        static const std::string LanternMatsetPath = "Skeletons/Items/Trinkets/Headlamp/lantern.polymatset";

        static const Color LanternlightColor = Color::FromBytes(255, 185, 130, 255);
        static const float LanternlightRange = 2500.0f;
        static const float LanternCenterLightRangePerc = 0.3f;
        static const float LanternCenterLightBrightnessPerc = 0.5f;
        static const float LanternlightShadowPerc = 0.6f;

        Lantern::Lantern(const ItemParameters& parameters)
            : Headlamp(parameters, "item_lantern_name", LanternSkeletonPath, LanternMatsetPath)
            , _centerLight(Vector2f::Zero, LanternlightRange * LanternCenterLightRangePerc, LanternlightColor * LanternCenterLightBrightnessPerc, LanternlightShadowPerc)
            , _largeLight(Vector2f::Zero, LanternlightRange, LanternlightColor, LanternlightShadowPerc)
        {
            SetTooltipDescription("item_lantern_description");
            SetTooltipFlavor("item_lantern_flavor");
            SetIcon(CookbookIconMatsetPath, "icon_lantern");
            AddSelectionSounds(GetMetalTrinketSelectionSounds());
        }

        void Lantern::OnUpdate(double totalTime, float dt)
        {
            Headlamp::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            const Vector2f& base = skeleton->GetJointPosition("light");

            float characterAlpha = GetColor().A / 255.0f;

            _centerLight.Position = base;
            _centerLight.LightColor = LanternlightColor * characterAlpha * GetLightIntensity();

            _largeLight.Position = base;
            _largeLight.LightColor = LanternlightColor * characterAlpha * GetLightIntensity();
        }

        void Lantern::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            Headlamp::OnDraw(levelRenderer);

            if (IsLightEnabled())
            {
                levelRenderer->AddLight(_centerLight);
                levelRenderer->AddLight(_largeLight);
            }
        }
    }

    template <>
    void EnumeratePreloads<Item::Headlamp>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicTrinket>(preloads);

        preloads.insert(Item::CookbookIconMatsetPath);
    }

    template <>
    void EnumeratePreloads<Item::BasicHeadlamp>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Headlamp>(preloads);

        preloads.insert(Item::BasicHeadlampSkeletonPath);
        preloads.insert(Item::BasicHeadlampMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::Flashlight>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Headlamp>(preloads);

        preloads.insert(Item::FlashlightSkeletonPath);
        preloads.insert(Item::FlashlightMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::Lantern>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Headlamp>(preloads);

        preloads.insert(Item::LanternSkeletonPath);
        preloads.insert(Item::LanternMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetMetalTrinketSelectionSounds());
    }
}
