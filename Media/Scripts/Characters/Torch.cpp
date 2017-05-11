#include "Characters/Torch.hpp"

#include "Drawables/Flame.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Character
    {
        const float TorchRadius = 1500.0f;
        const float TorchShadowPerc = 0.7f;

        FlameHolder::FlameHolder(const CharacterParameters& parameters, const std::string& skeleton, const std::string& materialSet,
                                 const std::string& lightJoint, const std::string& particleJoint, bool showParticles, const Color& color,
                                 bool castsShadows, bool startOn, bool playSounds)
            : SkeletonCharacter(parameters, skeleton, materialSet)
            , _flame(nullptr)
            , _lightJoint(lightJoint)
            , _particleJoint(particleJoint)
        {
            SetEntityMask(CharacterMask_Usable);
            SetMoveType(MoveType_None);

            DisablePhysics();
            SetInvulnerable(true);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);
            SetSkeletonRotatesWithDirection(true);

            _flame = std::make_shared<Graphics::Flame>(color, castsShadows, startOn, showParticles, playSounds, GetLevel()->GetSoundManager());
        }

        float FlameHolder::GetRadius() const
        {
            return _flame->GetRadius();
        }

        bool FlameHolder::IsOn() const
        {
            return _flame->IsOn();
        }

        void FlameHolder::TurnOn()
        {
            SetOn(true);
        }

        void FlameHolder::TurnOff()
        {
            SetOn(false);
        }

        void FlameHolder::SetOn(bool on)
        {
            if (on != _flame->IsOn())
            {
                _flame->SetState(on);
            }
        }

        void FlameHolder::Toggle()
        {
            SetOn(!_flame->IsOn());
        }

        void FlameHolder::OnLoadContent(Content::ContentManager* contentManager)
        {
            SkeletonCharacter::OnLoadContent(contentManager);
            _flame->LoadContent(contentManager);
            AttachDrawable(_particleJoint, _flame.get());
        }

        void FlameHolder::OnUnloadContent()
        {
            DetachDrawable(_particleJoint, _flame.get());
            _flame->UnloadContent();
            SkeletonCharacter::OnUnloadContent();
        }

        void FlameHolder::OnUpdate(double totalTime, float dt)
        {
            SkeletonCharacter::OnUpdate(totalTime, dt);
            _flame->SetScale(GetScale());
            _flame->SetPosition(GetSkeleton()->GetJointPosition(_lightJoint));
            _flame->Update(totalTime, dt);
        }

        void FlameHolder::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            SkeletonCharacter::OnDraw(levelRenderer);

            if (_flame->ShouldDrawLights())
            {
                levelRenderer->AddLight(_flame->GetCenterLight());
                levelRenderer->AddLight(_flame->GetMainLight());
            }
        }

        void FlameHolder::OnPostInteractedWith(Character* interactor)
        {
            Toggle();
        }

        void FlameHolder::SetLightRadius(float rad)
        {
            _flame->SetRadius(rad);
        }

        static const std::string FlameStartSoundPath = "Audio/Characters/Brazier/Brazier_Ignite.ogg";
        static const std::string FlameStopSoundPath = "";
        static const std::string FlameBurnSoundPath = "Audio/Characters/Brazier/Brazier_Burning.ogg";

        Torch::Torch(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/torch.skel", "Skeletons/Characters/Torch/torch.polymatset", "light", "fire_spawn",
                          true, color, castsShadows, startOn, playSounds)
        {
            AddMaterial("cage", "", 1.0f);
            AddMaterial("cage", "Skeletons/Characters/Torch/torch_cage_0.polymatset", 1.0f);
            AddMaterial("cage", "Skeletons/Characters/Torch/torch_cage_1.polymatset", 1.0f);
            AddMaterial("cage", "Skeletons/Characters/Torch/torch_cage_2.polymatset", 1.0f);
            AddMaterial("cage", "Skeletons/Characters/Torch/torch_cage_3.polymatset", 1.0f);

            SetAttachToGroundOnSpawn(false);

            SetTooltip("character_torch_name");
        }

        WallBrazier::WallBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/brazier.skel", "Skeletons/Characters/Torch/brazier.polymatset", "light", "particle",
                          true, color, castsShadows, startOn, playSounds)
        {
            SetAttachToGroundOnSpawn(false);

            SetTooltip("character_brazier_name");
        }

        FloorBrazier::FloorBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/brazier_floor.skel", "Skeletons/Characters/Torch/brazier_floor.polymatset", "light",
                          "particle", true, color, castsShadows, startOn, playSounds)
        {
            AddMaterial("stand", "Skeletons/Characters/Torch/brazier_floor_stand_0.polymatset", 1.0f);
            AddMaterial("stand", "Skeletons/Characters/Torch/brazier_floor_stand_1.polymatset", 1.0f);

            SetAttachToGroundOnSpawn(true);

            SetTooltip("character_brazier_name");
        }

        PotBrazier::PotBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/brazier_pot.skel", "Skeletons/Characters/Torch/brazier_pot.polymatset", "light", "particle",
                          true, color, castsShadows, startOn, playSounds)
        {
            SetAttachToGroundOnSpawn(true);

            SetTooltip("character_brazier_name");
        }

        HangingBrazier::HangingBrazier(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/brazier_hang.skel", "Skeletons/Characters/Torch/brazier_hang.polymatset", "light",
                          "particle", true, color, castsShadows, startOn, playSounds)
        {
            SetAttachToGroundOnSpawn(false);

            SetTooltip("character_brazier_name");
        }

        SmallCandle::SmallCandle(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/candle2.skel", "Skeletons/Characters/Torch/candle2.polymatset", "light",
                          "particle", false, color, castsShadows, startOn, playSounds)
        {
            SetLightRadius(TorchRadius * 0.3f);
            SetAttachToGroundOnSpawn(false);

            SetTooltip("character_candle_name");
        }

        LargeCandle::LargeCandle(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, "Skeletons/Characters/Torch/candle1.skel", "Skeletons/Characters/Torch/candle1.polymatset", "light",
                          "particle", false, color, castsShadows, startOn, playSounds)
        {
            SetLightRadius(TorchRadius * 0.3f);
            SetAttachToGroundOnSpawn(false);

            SetTooltip("character_candle_name");
        }

        static const std::string CampfireSkeletonPath = "Skeletons/Items/Doodads/firepit_1.skel";
        static const std::string CampfireMatsetPath = "Skeletons/Items/Doodads/firepit_1.polymatset";

        CampFire::CampFire(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, CampfireSkeletonPath, CampfireMatsetPath, "particle", "particle", true, color, castsShadows, startOn, playSounds)
        {
            SetAttachToGroundOnSpawn(true);

            SetTooltip("character_campfire_name");
        }

        void CampFire::OnSpawn()
        {
            FlameHolder::OnSpawn();

            PlayAnimation("idle", true, 0.0f, 0.0f);
        }

        static const std::string CookfireSkeletonPath = "Skeletons/Items/Doodads/firepit_1.skel";
        static const std::string CookfireMatsetPath = "Skeletons/Items/Doodads/firepit_2.polymatset";

        CookFire::CookFire(const CharacterParameters& parameters, const Color& color, bool castsShadows, bool startOn, bool playSounds)
            : FlameHolder(parameters, CookfireSkeletonPath, CookfireMatsetPath, "particle", "particle", true, color, castsShadows, startOn, playSounds)
        {
            SetAttachToGroundOnSpawn(true);

            SetTooltip("character_cookfire_name");
        }

        void CookFire::OnSpawn()
        {
            FlameHolder::OnSpawn();

            PlayAnimation("idle", true, 0.0f, 0.0f);
        }
    }

    template <>
    void EnumeratePreloads<Character::FlameHolder>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::SkeletonCharacter>(preloads);
        EnumeratePreloads<Graphics::Flame>(preloads);
    }

    template <>
    void EnumeratePreloads<Character::Torch>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);

        preloads.insert("Skeletons/Characters/Torch/torch.skel");
        preloads.insert("Skeletons/Characters/Torch/torch.polymatset");
        preloads.insert("Skeletons/Characters/Torch/torch_cage_0.polymatset");
        preloads.insert("Skeletons/Characters/Torch/torch_cage_1.polymatset");
        preloads.insert("Skeletons/Characters/Torch/torch_cage_2.polymatset");
        preloads.insert("Skeletons/Characters/Torch/torch_cage_3.polymatset");

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::WallBrazier>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/brazier.skel");
        preloads.insert("Skeletons/Characters/Torch/brazier.polymatset");

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::FloorBrazier>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/brazier_floor.skel");
        preloads.insert("Skeletons/Characters/Torch/brazier_floor.polymatset");
        preloads.insert("Skeletons/Characters/Torch/brazier_floor_stand_0.polymatset");
        preloads.insert("Skeletons/Characters/Torch/brazier_floor_stand_1.polymatset");

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::HangingBrazier>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/brazier_hang.skel");
        preloads.insert("Skeletons/Characters/Torch/brazier_hang.polymatset");

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::PotBrazier>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/brazier_pot.skel");
        preloads.insert("Skeletons/Characters/Torch/brazier_pot.polymatset");

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::SmallCandle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/candle2.skel");
        preloads.insert("Skeletons/Characters/Torch/candle2.polymatset");
    }

    template <>
    void EnumeratePreloads<Character::LargeCandle>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert("Skeletons/Characters/Torch/candle1.skel");
        preloads.insert("Skeletons/Characters/Torch/candle1.polymatset");
    }

    template <>
    void EnumeratePreloads<Character::CampFire>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert(Character::CampfireSkeletonPath);
        preloads.insert(Character::CampfireMatsetPath);

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }

    template <>
    void EnumeratePreloads<Character::CookFire>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::FlameHolder>(preloads);
        preloads.insert(Character::CookfireSkeletonPath);
        preloads.insert(Character::CookfireMatsetPath);

        preloads.insert(Character::FlameStartSoundPath);
        preloads.insert(Character::FlameStopSoundPath);
        preloads.insert(Character::FlameBurnSoundPath);
    }
}
