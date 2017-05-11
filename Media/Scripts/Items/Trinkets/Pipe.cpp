#include "Items/Trinkets/Pipe.hpp"
#include "Characters/CharacterTraits.hpp"
#include "Items/ItemSounds.hpp"

#include "Random.hpp"

#include "ContentUtility.hpp"

namespace Dwarf
{
    namespace Item
    {
        static const std::string PipeSkeletonPath = "Skeletons/Items/Trinkets/Pipe/pipe.skel";
        static const std::string PipeParticlesPath = "Particles/pipe_smoke.partsys";
        static const std::string PipeIconMatsetPath = "HUD/items.hudmatset";

        static const std::string PipeParticlesAttachJoint = "particles";
        static const std::string PipeParticlesAttachDirectionJoint = "particles_dir";

        static const std::pair<float, float> SmokePuffTimeRange = std::make_pair(2.5f, 4.0f);

        Pipe::Pipe(const ItemParameters& parameters, const std::string& nameCode, const std::string& matsetPath)
            : BasicTrinket(parameters, nameCode, PipeSkeletonPath, matsetPath)
            , _nextPuffTimer(0.0f)
            , _smoke(nullptr)
        {
        }

        void Pipe::OnEquip(Character::Character* newOwner)
        {
            Character::Smoker* smoker = AsA<Character::Smoker>(newOwner);
            if (smoker != nullptr)
            {
                SetOwnerSkeletonAttachPoint(smoker->GetPipeAttachPointName(), "origin", "b");
            }
            else
            {
                SetOwnerSkeletonAttachPoint("", "", "");
            }

            BasicTrinket::OnEquip(newOwner);
        }

        void Pipe::OnUnEquip(Character::Character* oldOwner)
        {
            BasicTrinket::OnUnEquip(oldOwner);
        }

        void Pipe::OnUpdate(double totalTime, float dt)
        {
            BasicTrinket::OnUpdate(totalTime, dt);

            _nextPuffTimer -= dt;
            if (HasOwner() && _nextPuffTimer <= 0.0f)
            {
                const auto& skeleton = GetSkeleton();
                const Vector2f& particlePos = skeleton->GetJointPosition(PipeParticlesAttachJoint);
                _smoke->SetPointSpawner(particlePos);

                _smoke->Burst();

                _nextPuffTimer = Random::RandomBetween(SmokePuffTimeRange.first, SmokePuffTimeRange.second);
            }

            _smoke->Update(totalTime, dt);
        }

        void Pipe::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicTrinket::OnDraw(levelRenderer);
        }

        void Pipe::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicTrinket::OnLoadContent(contentManager);

            _smoke = Content::CreateParticleSystemInstance(contentManager, PipeParticlesPath);
            GetSkeleton()->AddChildDrawable(PipeParticlesAttachJoint, _smoke);
        }

        void Pipe::OnUnloadContent()
        {
            GetSkeleton()->RemoveChildDrawable(PipeParticlesAttachJoint, _smoke);
            SafeRelease(_smoke);

            BasicTrinket::OnUnloadContent();
        }

        static const std::string BriarPipeMatsetPath = "Skeletons/Items/Trinkets/Pipe/briar.polymatset";

        BriarPipe::BriarPipe(const ItemParameters& parameters)
            : Pipe(parameters, "item_briar_pipe_name", BriarPipeMatsetPath)
            , _enemyHealthThreshold(0.3f)
        {
            SetTooltipDescription("item_briar_pipe_description", _enemyHealthThreshold * 100.0f);
            SetTooltipFlavor("item_briar_pipe_flavor");
            SetIcon(PipeIconMatsetPath, "icon_briar_pipe");
            AddSelectionSounds(GetWoodTrinketSelectionSounds());
        }

        Character::Damage BriarPipe::OnPreInflictDamage(Character::Character* target, const Vector2f& position,
                                                        const Character::Damage& inputDamage)
        {
            Character::Damage result(inputDamage);

            if (target && target->GetHealth().GetPercent() <= _enemyHealthThreshold)
            {
                result.CriticalChance = 1.0f;
            }

            return result;
        }

        static const std::string MeerschaumPipeMatsetPath = "Skeletons/Items/Trinkets/Pipe/meerschaum.polymatset";

        MeerschaumPipe::MeerschaumPipe(const ItemParameters& parameters)
            : Pipe(parameters, "item_meerschaum_pipe_name", MeerschaumPipeMatsetPath)
        {
            SetTooltipDescription("item_meerschaum_pipe_description");
            SetTooltipFlavor("item_meerschaum_pipe_flavor");
            SetIcon(PipeIconMatsetPath, "icon_meerschaum_pipe");
            AddSelectionSounds(GetWoodTrinketSelectionSounds());
        }

        Character::Damage MeerschaumPipe::OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage)
        {
            Character::Damage result(inputDamage);

            if (target && target->GetState() == Character::CharacterState_Idle)
            {
                result.CriticalChance = 1.0f;
            }

            return result;
        }

        static const std::string SteinPipeMatsetPath = "Skeletons/Items/Trinkets/Pipe/stein.polymatset";

        SteinPipe::SteinPipe(const ItemParameters& parameters)
            : Pipe(parameters, "item_stein_pipe_name", SteinPipeMatsetPath)
        {
            SetTooltipDescription("item_stein_pipe_description");
            SetTooltipFlavor("item_stein_pipe_flavor");
            SetIcon(PipeIconMatsetPath, "icon_stein_pipe");
            AddSelectionSounds(GetWoodTrinketSelectionSounds());
        }
    }

    template <>
    void EnumeratePreloads<Item::Pipe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicTrinket>(preloads);

        preloads.insert(Item::PipeSkeletonPath);
        preloads.insert(Item::PipeParticlesPath);
    }

    template <>
    void EnumeratePreloads<Item::BriarPipe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pipe>(preloads);

        preloads.insert(Item::BriarPipeMatsetPath);
        preloads.insert(Item::PipeIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::MeerschaumPipe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pipe>(preloads);

        preloads.insert(Item::MeerschaumPipeMatsetPath);
        preloads.insert(Item::PipeIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodTrinketSelectionSounds());
    }

    template <>
    void EnumeratePreloads<Item::SteinPipe>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::Pipe>(preloads);

        preloads.insert(Item::SteinPipeMatsetPath);
        preloads.insert(Item::PipeIconMatsetPath);

        EnumerateSoundVectorPreloads(preloads, Item::GetWoodTrinketSelectionSounds());
    }
}
