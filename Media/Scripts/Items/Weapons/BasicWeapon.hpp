#pragma once

#include "Item/Weapon.hpp"
#include "Character/Character.hpp"
#include "Physics/SkeletonCollision.hpp"
#include "Level/LevelInstance.hpp"

#include "Items/Weapons/WeaponTraits.hpp"
#include "DamageTypes.hpp"
#include "SkeletonUtility.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BasicWeapon : public Weapon, public SkeletonHaver
        {
        public:
            BasicWeapon(const ItemParameters& parameters, const std::string& nameCode, WeaponType type, const std::string& skeletonPath, const std::string& matsetPath);

            virtual float GetRange() const override;
            virtual const Character::Damage& GetDPS() const override;

            virtual const BlockMap& GetBlockChances() const override;

            virtual const Rectanglef& GetBounds() const override;
            virtual Physics::Collision* GetCollision() const override;

            // Properties
            virtual float GetArmorValue() const override;

            // Events
            virtual Character::Damage OnPreRecieveDamage(Character::Character* source, const Vector2f& position, const Character::Damage& inputDamage) override;

            // HUD
            virtual const HUD::PanelDrawable* GetIcon() const override;
            virtual const HUD::Panel* GetTooltip() const override;

            virtual const Audio::Sound* GetSelectionSound() const override;

            // Skeleton
            Animation::SkeletonInstance* GetSkeleton() const override;

        protected:
            virtual ~BasicWeapon();

            void SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetTooltipDescription(const std::string codeName, ...);
            void SetTooltipFlavor(const std::string codeName);

            void SetAttachPoints(const std::string& weaponAttachA, const std::string& weaponAttachB);
            void SetDamageJoint(const std::string& damageJoint);
            void SetDPS(const Character::Damage& dps);

            void SetArmorAmmount(float armor);
            void SetBlockChance(Character::DamageType type, float chance, float block);

            void BreakMaterial(const std::string& material);

            virtual void SetMaterialCollisionSound(const Audio::SoundPathVector& sounds);

            void AddSelectionSounds(const Audio::SoundPathVector& sounds);

            virtual void OnEquip(Character::Character* newOwner) override;
            virtual void OnUnEquip(Character::Character* oldOwner) override;

            virtual void OnAttachPointChanged(const std::string& oldAttachPoint, const std::string& newAttachPoint) override;

            virtual void OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos) override;
            virtual void OnScaleChange(float oldScale, float newScale) override;
            virtual void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override;
            virtual void OnColorChange(const Color& oldCol, const Color& newCol) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnPostInteractedWith(Character::Character* interactor) override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

            void SetSkeleton(Animation::SkeletonInstance* skeleton);

        private:
            std::string _iconMatsetPath;
            std::string _iconMaterialName;

            std::string _description;
            std::string _flavor;
            HUD::PanelDrawable* _icon;
            HUD::Panel* _tooltip;

            std::string _weaponAttachPointA;
            std::string _weaponAttachPointB;

            std::string _damageJoint;
            Character::Damage _dps;

            float _armor;
            BlockMap _blocks;

            std::string _skeletonPath;
            std::string _matsetPath;
            Animation::SkeletonInstance* _skeleton;
            Physics::SkeletonCollision* _collision;

            Audio::SoundSet _selectionSounds;
            std::pair<float, float> _pickupSoundRadius;
            float _pickupSoundVolume;

            std::vector<Animation::BrokenMaterial> _brokenPieces;

            float _physicsSoundResetTimer = 0.0f;
            Audio::SoundSet _physicsMaterialSounds;
        };
    }

    template <>
    void EnumeratePreloads<Item::BasicWeapon>(PreloadSet& preloads);
}
