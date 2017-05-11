#include "Items/Weapons/LightningSword.hpp"

namespace Dwarf
{
    namespace Item
    {
        LightningSword::LightningSword(const ItemParameters& parameters)
            : BasicMeleeWeapon(parameters, WeaponType_Melee_1H, "Skeletons/Items/Weapons/LightningSword/lighting_sword.skel", "Skeletons/Items/Weapons/LightningSword/lightning_sword.polymatset")
            , _lightningParticles(NULL)
            , _light(Vector2f::Zero, Vector2f::Zero, 1.0f, 1.0f, Color::White, 0.0f)
        {
            SetAttachPoints("a", "b");
        }

        void LightningSword::OnLoadContent(Content::ContentManager* contentManager)
        {
            BasicWeapon::OnLoadContent(contentManager);

            const Particles::ParticleSystem* lightningParticles = contentManager->Load<Particles::ParticleSystem>("Particles/weapon_lightning.partsys");
            _lightningParticles = new Particles::ParticleSystemInstance(lightningParticles);
            SafeRelease(lightningParticles);

            _lightningParticles->Start();
        }

        void LightningSword::OnUnloadContent()
        {
            BasicWeapon::OnUnloadContent();

            SafeRelease(_lightningParticles);
        }

        void LightningSword::OnUpdate(double totalTime, float dt)
        {
            BasicWeapon::OnUpdate(totalTime, dt);

            Animation::SkeletonInstance* skeleton = GetSkeleton();
            const Vector2f& start = skeleton->GetJointPosition("blade_base");
            const Vector2f& end = skeleton->GetJointPosition("blade_extent");
            const Vector2f& radius = skeleton->GetJointPosition("blade_light_radius");

            _lightningParticles->SetLineSpawner(start, end);
            _lightningParticles->Update(totalTime, dt);

            _light.Start = start;
            _light.End = end;
            _light.StartRadius = Vector2f::Distance(end, radius);
            _light.EndRadius = Vector2f::Distance(end, radius);
        }

        void LightningSword::OnDraw(Graphics::LevelRenderer* levelRenderer) const
        {
            BasicWeapon::OnDraw(levelRenderer);

            levelRenderer->AddDrawable(_lightningParticles, false);
            levelRenderer->AddLight(_light);
        }

    }

    template <>
    void EnumeratePreloads<Item::LightningSword>(PreloadSet& preloads)
    {
        EnumeratePreloads<Item::BasicMeleeWeapon>(preloads);

        preloads.insert("Skeletons/Items/Weapons/LightningSword/lighting_sword.skel");
        preloads.insert("Skeletons/Items/Weapons/LightningSword/lightning_sword.polymatset");
        preloads.insert("Particles/weapon_lightning.partsys");
    }
}
