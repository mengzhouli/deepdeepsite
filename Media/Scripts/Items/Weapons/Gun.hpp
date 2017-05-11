#pragma once

#include "Particles/ParticleSystemInstance.hpp"

#include "Items/Weapons/BasicRangedWeapon.hpp"
#include "Items/Weapons/WeaponTraits.hpp"

namespace Dwarf
{
    namespace Item
    {
        class Gun : public BasicRangedWeapon, public MuzzleHaver
        {
        public:
            Gun(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skelPath, const std::string& matsetPath,
                const std::string& muzzleJointName, const std::string& muzzleDirJointName);

            virtual Rayf GetMuzzlePosition() const override;

        protected:
            void SetShellParticleJoint(const std::string& particlesPath, const std::string& baseJoint, const std::string& directionJoint);

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void Fire(const Character::Damage& dmg) override;

            virtual void OnUpdate(double totalTime, float dt) override;
            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            Particles::ParticleSystemInstance* _explosion;
            Particles::ParticleSystemInstance* _smoke;

            std::string _shellsPath;
            Particles::ParticleSystemInstance* _shells = nullptr;
            std::pair<std::string, std::string> _shellsJoints;

            std::string _muzzleName;
            std::string _muzzleDirName;

            const float _blastLightDuration;
            float _blastLightTimer;
            Lights::PointLight _explosionLight;
            Lights::LineLight _targetLight;
        };

        class BasicRifle : public Gun
        {
        public:
            BasicRifle(const ItemParameters& parameters);
        };

        class HandCannonRifle : public Gun
        {
        public:
            HandCannonRifle(const ItemParameters& parameters);
        };

        class BlunderbussRifle : public Gun
        {
        public:
            BlunderbussRifle(const ItemParameters& parameters);

            Character::Damage OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage) override;

        private:
            float _maxDamageReduction = 0.0f;
        };

        class BlackQueenRifle : public Gun
        {
        public:
            BlackQueenRifle(const ItemParameters& parameters);
        };

        class Pistol : public Gun, public GrappleAttachable, public FlareAttachable
        {
        public:
            Pistol(const ItemParameters& parameters, const std::string& nameCode, const std::string& skelPath, const std::string& matsetPath,
                   const std::string& muzzleJointName, const std::string& muzzleDirJointName);

            virtual void SetGrappleAttached(bool attached) override;
            virtual void SetFlareAttached(bool attached) override;

        protected:
            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

        private:
            const Graphics::PolygonMaterialSet* _grappleMaterial;
            const Graphics::PolygonMaterialSet* _flareMaterial;
        };

        class BasicPistol : public Pistol
        {
        public:
            BasicPistol(const ItemParameters& parameters);
        };

        class FingerCannonPistol : public Pistol
        {
        public:
            FingerCannonPistol(const ItemParameters& parameters);
        };

        class BlunderbussPistol : public Pistol
        {
        public:
            BlunderbussPistol(const ItemParameters& parameters);

            Character::Damage OnPreInflictDamage(Character::Character* target, const Vector2f& position, const Character::Damage& inputDamage) override;

        private:
            float _maxDamageReduction = 0.0f;
        };

        class BlackPrincessPistol : public Pistol
        {
        public:
            BlackPrincessPistol(const ItemParameters& parameters);
        };
    }

    template <>
    void EnumeratePreloads<Item::Gun>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BasicRifle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::HandCannonRifle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BlunderbussRifle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BlackQueenRifle>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::Pistol>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BasicPistol>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::FingerCannonPistol>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BlunderbussPistol>(PreloadSet& preloads);

    template <>
    void EnumeratePreloads<Item::BlackPrincessPistol>(PreloadSet& preloads);
}
