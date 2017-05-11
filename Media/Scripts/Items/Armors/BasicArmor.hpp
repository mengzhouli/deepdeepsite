#pragma once

#include "Item/Armor.hpp"
#include "Level/LevelInstance.hpp"
#include "Graphics/MaterialSet.hpp"
#include "Physics/SkeletonCollision.hpp"

#include "Items/Weapons/WeaponTraits.hpp"
#include "SoundSet.hpp"

namespace Dwarf
{
    namespace Item
    {
        class BasicArmor : public Armor, public SkeletonHaver
        {
        public:
            BasicArmor(const ItemParameters& parameters, const std::string& nameCode, const std::string& matsetPath,
                       const std::string& skeletonPath, const std::string& skeletonMatsetPath);

            virtual float GetArmorValue() const override;

            virtual Character::MaterialType GetMaterial() const override;

            virtual const Rectanglef& GetBounds() const override;
            virtual Physics::Collision* GetCollision() const override;

            // HUD
            virtual const HUD::PanelDrawable* GetIcon() const override;
            virtual const HUD::Panel* GetTooltip() const override;

            virtual const Audio::Sound* GetSelectionSound() const override;

            // Skeleton
            Animation::SkeletonInstance* GetSkeleton() const override;

        protected:
            virtual ~BasicArmor();

            void SetArmorValue(float armor);
            void SetMaterial(Character::MaterialType material);

            void SetIcon(const std::string& iconMatsetPath, const std::string& iconMaterialName);
            void SetTooltipDescription(const std::string codeName, ...);
            void SetTooltipFlavor(const std::string& codeName);

            void AddSelectionSounds(const Audio::SoundPathVector& sounds);
            void AddPickupSounds(const Audio::SoundPathVector& sounds);

            virtual void OnEquip(Character::Character* newOwner) override;
            virtual void OnUnEquip(Character::Character* oldOwner) override;

            virtual void OnPositionChange(const Vector2f& oldPos, const Vector2f& newPos) override;
            virtual void OnScaleChange(float oldScale, float newScale) override;
            virtual void OnRotationChange(const Rotatorf& oldRot, const Rotatorf& newRot) override;
            virtual void OnColorChange(const Color& oldCol, const Color& newCol) override;

            virtual void OnPostInteractedWith(Character::Character* interactor) override;

            virtual void OnLoadContent(Content::ContentManager* contentManager) override;
            virtual void OnUnloadContent() override;

            virtual void OnUpdate(double totalTime, float dt) override;

            virtual void OnDraw(Graphics::LevelRenderer* levelRenderer) const override;

        private:
            float _armorValue;

            Character::MaterialType _material;

            std::string _iconMatsetPath;
            std::string _iconMaterialName;

            std::string _description;
            std::string _flavor;
            HUD::PanelDrawable* _icon;
            HUD::Panel* _tooltip;

            std::string _skeletonPath;
            std::string _skeletonMatsetPath;
            Animation::SkeletonInstance* _skeleton;
            Physics::SkeletonCollision* _collision;

            Audio::SoundSet _selectionSounds;
            Audio::SoundSet _pickupSounds;
            std::pair<float, float> _pickupSoundRadius;
            float _pickupSoundVolume;

            std::string _matsetPath;
            const Graphics::PolygonMaterialSet* _matset;
        };
    }

    template <>
    void EnumeratePreloads<Item::BasicArmor>(PreloadSet& preloads);
}
