#include "Characters/NecroKnight.hpp"

namespace Dwarf
{
    namespace Character
    {
        static const std::string NecroKnightSkeletonPath = "Skeletons/Characters/necro/necro.skel";
        static const std::string NecroKnightMaterialPath = "Skeletons/Characters/necro/necro.polymatset";
        static const std::string NecroKnightParticlePath = "Particles/phantom_fire.partsys";

        static const std::string NecroKnightParticleJointName = "particle_emitter";

        NecroKnight::NecroKnight(const CharacterParameters& parameters)
            : BasicCharacter(parameters, NecroKnightSkeletonPath, NecroKnightMaterialPath)
            , _particles(NULL)
        {
            SetItemSlotCount(Item::ItemSlot::Weapon, 1);

            SetMoveType(MoveType_Walk);
            SetMoveSpeed(200.0f);
            SetMaxHealth(30.0f);
            SetHealthRegeneration(0.0f);

            SetSkeletonCastsShadows(false);
            SetSkeletonScale(1.0f);
            SetSkeletonColor(Color::White);

            AddTerrainAnimation(AnimationType_Idle, Pathfinding::EdgeType_Walk, Pathfinding::TerrainType::All, -PiOver2, AnimationVariant_Standard, "idle_1", 1.0f);

            SetAggroRange(1000.0f);

            SetHeadName("joint_22");
        }

        void NecroKnight::OnUpdate(double totalTime, float dt)
        {
            BasicCharacter::OnUpdate(totalTime, dt);

            // TODO: POLYGONPARTICLES
            Rayf particleAttachPoint = GetAttachPoint(NecroKnightParticleJointName);
            _particles->SetPointSpawner(particleAttachPoint.Position);
            _particles->Update(totalTime, dt);
        }

        void NecroKnight::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicCharacter::OnDraw(levelRenderer);

            // Skeleton attachment will draw the particles
        }

        void NecroKnight::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicCharacter::OnLoadContent(contentManager);

            const Particles::ParticleSystem* particles = contentManager->Load<Particles::ParticleSystem>(NecroKnightParticlePath);
            _particles = new Particles::ParticleSystemInstance(particles);
            SafeRelease(particles);

            AttachDrawable(NecroKnightParticleJointName, _particles);
            _particles->SetRotation(Rotatorf::Pi);
            _particles->SetScale(1.25f);
            _particles->Start();
        }

        void NecroKnight::OnUnloadContent()
        {
            BasicCharacter::OnUnloadContent();

            SafeRelease(_particles);
        }

        void NecroKnight::OnStateChange(CharacterState newState)
        {
            if (newState == CharacterState_Dead)
            {
                SetSkeletonJointStrength(0);
                _particles->Stop();
            }

            BasicCharacter::OnStateChange(newState);
        }
    }

    template <>
    void EnumeratePreloads<Character::NecroKnight>(PreloadSet& preloads)
    {
        EnumeratePreloads<Character::BasicCharacter>(preloads);
        preloads.insert(Character::NecroKnightSkeletonPath);
        preloads.insert(Character::NecroKnightMaterialPath);
        preloads.insert(Character::NecroKnightParticlePath);
    }
}
